#include <metal_stdlib>
using namespace metal;

// ---------------------------------------------------------------------------
// Structs
// ---------------------------------------------------------------------------

struct Ray {
    float3 origin;
    float3 direction;
    float minDistance;
    float maxDistance;
};

struct HitResult {
    float  distance;
    float3 normal;
    uint   primitiveIndex;
    bool   hit;
};

struct Particle {
    float3 position;
    float3 velocity;
    float  mass;
    float  charge;
};

struct Vertex {
    packed_float3 position;
    packed_float3 normal;
    float2        uv;
};

struct Camera {
    float4x4 inverseViewProjection;
    float3   position;
    uint     width;
    uint     height;
};

// ---------------------------------------------------------------------------
// 1. Ray tracing kernel
// ---------------------------------------------------------------------------

kernel void trace_primary_rays(
    raytracing::primitive_acceleration_structure accelerationStructure [[buffer(0)]],
    constant Camera& camera                               [[buffer(1)]],
    device HitResult* hitResults                          [[buffer(2)]],
    texture2d<float, access::write> outTexture            [[texture(0)]],
    uint2 tid                                             [[thread_position_in_grid]]
) {
    if (tid.x >= camera.width || tid.y >= camera.height) return;

    float2 ndc = float2(tid) / float2(camera.width, camera.height) * 2.0 - 1.0;
    float4 worldTarget = camera.inverseViewProjection * float4(ndc, 1.0, 1.0);
    float3 dir = normalize(worldTarget.xyz / worldTarget.w - camera.position);

    raytracing::ray r;
    r.origin = camera.position;
    r.direction = dir;
    r.min_distance = 0.001;
    r.max_distance = 1000.0;

    raytracing::intersector<raytracing::triangle_data> inter;
    auto result = inter.intersect(r, accelerationStructure);

    float4 color;
    if (result.type == raytracing::intersection_type::triangle) {
        float t = result.distance;
        color = float4(t * 0.01, 1.0 - t * 0.01, 0.5, 1.0);
    } else {
        color = float4(0.1, 0.1, 0.3, 1.0);
    }

    uint idx = tid.y * camera.width + tid.x;
    hitResults[idx].distance = result.distance;
    hitResults[idx].hit = (result.type != raytracing::intersection_type::none);
    outTexture.write(color, tid);
}

// ---------------------------------------------------------------------------
// 2. Argument buffer — bindless texture array
// ---------------------------------------------------------------------------

kernel void sample_bindless_textures(
    array<texture2d<float, access::sample>, 16> textures [[texture(0)]],
    device const uint* textureIndices                    [[buffer(0)]],
    device float4* output                                [[buffer(1)]],
    sampler samp                                         [[sampler(0)]],
    uint tid                                             [[thread_position_in_grid]]
) {
    uint texIdx = textureIndices[tid] & 0xF;
    float2 uv = float2(0.5, 0.5);
    float4 sample = textures[texIdx].sample(samp, uv);
    output[tid] = sample;
}

// ---------------------------------------------------------------------------
// 3. Atomic float operations — parallel accumulation
// ---------------------------------------------------------------------------

kernel void atomic_float_accumulate(
    device const float* values         [[buffer(0)]],
    device atomic<float>* accumulators [[buffer(1)]],
    device const uint* binIndices      [[buffer(2)]],
    uint tid                           [[thread_position_in_grid]]
) {
    float val = values[tid];
    uint bin = binIndices[tid];

    float absVal = abs(val);
    float contribution = absVal * absVal;

    atomic_fetch_add_explicit(&accumulators[bin], contribution, memory_order_relaxed);
}

// ---------------------------------------------------------------------------
// 4. SIMD group reductions — warp-level programming
// ---------------------------------------------------------------------------

kernel void simd_group_analytics(
    device const float* input   [[buffer(0)]],
    device float* results       [[buffer(1)]],
    uint tid                    [[thread_position_in_grid]],
    uint simd_lane              [[thread_index_in_simdgroup]],
    uint simd_group_id          [[simdgroup_index_in_threadgroup]],
    uint simd_size              [[threads_per_simdgroup]]
) {
    float val = input[tid];

    float groupSum = simd_sum(val);
    float groupMin = simd_min(val);
    float groupMax = simd_max(val);

    bool isPositive = val > 0.0;
    simd_vote::vote_t ballot = simd_ballot(isPositive);

    float first = simd_broadcast(val, 0);
    float range = groupMax - groupMin;

    if (simd_lane == 0) {
        uint base = simd_group_id * 4;
        results[base + 0] = groupSum;
        results[base + 1] = groupMin;
        results[base + 2] = groupMax;
        results[base + 3] = range + first;
    }
}

// ---------------------------------------------------------------------------
// 5. Cooperative matrix multiply — simdgroup matrix ops
// ---------------------------------------------------------------------------

kernel void simdgroup_matmul(
    device const float* A   [[buffer(0)]],
    device const float* B   [[buffer(1)]],
    device float* C         [[buffer(2)]],
    constant uint& M        [[buffer(3)]],
    constant uint& N        [[buffer(4)]],
    constant uint& K        [[buffer(5)]],
    uint2 group_pos         [[threadgroup_position_in_grid]],
    uint simd_lane          [[thread_index_in_simdgroup]]
) {
    simdgroup_float8x8 a_tile;
    simdgroup_float8x8 b_tile;
    simdgroup_float8x8 c_tile;

    // Initialize accumulator to zero
    simdgroup_load(c_tile, C + group_pos.y * 8 * N + group_pos.x * 8, N);

    for (uint k = 0; k < K; k += 8) {
        simdgroup_load(a_tile, A + group_pos.y * 8 * K + k, K);
        simdgroup_load(b_tile, B + k * N + group_pos.x * 8, N);
        simdgroup_multiply_accumulate(c_tile, a_tile, b_tile, c_tile);
    }

    simdgroup_store(c_tile, C + group_pos.y * 8 * N + group_pos.x * 8, N);
}

// ---------------------------------------------------------------------------
// 6. Multi-sample texture resolve
// ---------------------------------------------------------------------------

kernel void resolve_msaa(
    texture2d_ms<float, access::read> msTexture  [[texture(0)]],
    texture2d<float, access::write> resolved     [[texture(1)]],
    uint2 tid                                    [[thread_position_in_grid]]
) {
    uint sampleCount = msTexture.get_num_samples();
    float4 accum = float4(0.0);

    for (uint s = 0; s < sampleCount; ++s) {
        accum += msTexture.read(tid, s);
    }

    float4 averaged = accum / float(sampleCount);
    resolved.write(averaged, tid);
}

// ---------------------------------------------------------------------------
// 7. Packed types — vertex processing
// ---------------------------------------------------------------------------

kernel void unpack_vertices(
    device const Vertex* vertices   [[buffer(0)]],
    device float4* positions        [[buffer(1)]],
    device float4* normals          [[buffer(2)]],
    uint tid                        [[thread_position_in_grid]]
) {
    // packed_float3 -> float3 implicit conversion
    float3 pos = float3(vertices[tid].position);
    float3 nrm = float3(vertices[tid].normal);

    nrm = normalize(nrm);

    // Swizzle and pack into float4 for aligned output
    positions[tid] = float4(pos, 1.0);
    normals[tid] = float4(nrm, 0.0);
}

// ---------------------------------------------------------------------------
// 8. Half-precision compute
// ---------------------------------------------------------------------------

kernel void half_precision_sdf(
    device const half4* pointsA     [[buffer(0)]],
    device const half4* pointsB     [[buffer(1)]],
    device half* distances          [[buffer(2)]],
    uint tid                        [[thread_position_in_grid]]
) {
    half4 a = pointsA[tid];
    half4 b = pointsB[tid];

    half3 diff = a.xyz - b.xyz;
    half distSq = dot(diff, diff);
    half dist = sqrt(distSq);

    // Smooth-minimum blend with second implicit surface
    half radius = 0.5h;
    half sdf1 = dist - radius;
    half sdf2 = length(a.xyz) - 1.0h;

    half k = 4.0h;
    half h = clamp(half(0.5h + 0.5h * (sdf2 - sdf1) / k), half(0.0h), half(1.0h));
    half blended = mix(sdf2, sdf1, h) - k * h * (1.0h - h);

    distances[tid] = blended;
}

// ---------------------------------------------------------------------------
// 9. Imageblock tile shading
// ---------------------------------------------------------------------------

struct TileData {
    float4 color [[color(0)]];
};

kernel void tile_average_blur(
    imageblock<TileData> tileBlock,
    ushort2 tid [[thread_position_in_threadgroup]]
) {
    // Read center pixel from imageblock
    TileData center = tileBlock.read(tid);
    float4 accum = center.color;
    float count = 1.0;

    // Simple 3x3 box filter within the tile
    for (short dy = -1; dy <= 1; ++dy) {
        for (short dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            ushort2 neighbor = ushort2(short2(tid) + short2(dx, dy));
            TileData sample = tileBlock.read(neighbor);
            accum += sample.color;
            count += 1.0;
        }
    }

    TileData result;
    result.color = accum / count;
    tileBlock.write(result, tid);
}

// ---------------------------------------------------------------------------
// 10. Visible function table — indirect dispatch
// ---------------------------------------------------------------------------

[[visible]]
float shade_diffuse(float3 normal, float3 lightDir) {
    return saturate(dot(normal, lightDir));
}

[[visible]]
float shade_specular(float3 normal, float3 lightDir) {
    float3 reflected = reflect(-lightDir, normal);
    float3 viewDir = float3(0.0, 0.0, 1.0);
    return powr(saturate(dot(reflected, viewDir)), 32.0);
}

using shade_fn = float(float3, float3);

kernel void indirect_shading(
    device const float3* normals                [[buffer(0)]],
    device const uint* shadingModes             [[buffer(1)]],
    device float* results                       [[buffer(2)]],
    constant float3& lightDirection             [[buffer(3)]],
    visible_function_table<shade_fn> shadeTable [[buffer(4)]],
    uint tid                                    [[thread_position_in_grid]]
) {
    float3 n = normalize(normals[tid]);
    float3 l = normalize(lightDirection);

    uint mode = shadingModes[tid];
    float shading = shadeTable[mode](n, l);

    results[tid] = shading;
}
