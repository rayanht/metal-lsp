#include <metal_stdlib>
using namespace metal;

// ---------------------------------------------------------------------------
// 1. Parallel reduction — sum an array using threadgroup shared memory
// ---------------------------------------------------------------------------
kernel void parallel_reduce_sum(
    device const float* input  [[buffer(0)]],
    device float*       output [[buffer(1)]],
    uint tid   [[thread_position_in_grid]],
    uint lid   [[thread_position_in_threadgroup]],
    uint gid   [[threadgroup_position_in_grid]],
    uint tpg   [[threads_per_threadgroup]]
) {
    threadgroup float shared[256];
    shared[lid] = input[tid];
    threadgroup_barrier(mem_flags::mem_threadgroup);

    for (uint stride = tpg / 2; stride > 0; stride >>= 1) {
        if (lid < stride) {
            shared[lid] += shared[lid + stride];
        }
        threadgroup_barrier(mem_flags::mem_threadgroup);
    }

    if (lid == 0) {
        output[gid] = shared[0];
    }
}

// ---------------------------------------------------------------------------
// 2. Tiled matrix multiplication using threadgroup memory and float4x4
// ---------------------------------------------------------------------------
constant uint TILE_SIZE = 16;

kernel void tiled_matmul(
    device const float* A      [[buffer(0)]],
    device const float* B      [[buffer(1)]],
    device float*       C      [[buffer(2)]],
    constant uint&      N      [[buffer(3)]],
    uint2 gid  [[thread_position_in_grid]],
    uint2 lid  [[thread_position_in_threadgroup]]
) {
    threadgroup float tileA[TILE_SIZE][TILE_SIZE];
    threadgroup float tileB[TILE_SIZE][TILE_SIZE];

    float sum = 0.0f;
    uint row = gid.y;
    uint col = gid.x;

    for (uint t = 0; t < N; t += TILE_SIZE) {
        tileA[lid.y][lid.x] = A[row * N + (t + lid.x)];
        tileB[lid.y][lid.x] = B[(t + lid.y) * N + col];
        threadgroup_barrier(mem_flags::mem_threadgroup);

        for (uint k = 0; k < TILE_SIZE; ++k) {
            sum += tileA[lid.y][k] * tileB[k][lid.x];
        }
        threadgroup_barrier(mem_flags::mem_threadgroup);
    }

    C[row * N + col] = sum;
}

// ---------------------------------------------------------------------------
// 3. Exclusive prefix scan using simd_shuffle_up
// ---------------------------------------------------------------------------
kernel void exclusive_prefix_scan(
    device float* data   [[buffer(0)]],
    constant uint& count [[buffer(1)]],
    uint tid       [[thread_position_in_grid]],
    uint lane      [[thread_index_in_simdgroup]],
    uint simd_size [[threads_per_simdgroup]]
) {
    float val = (tid < count) ? data[tid] : 0.0f;

    // Up-sweep within the SIMD group
    for (uint offset = 1; offset < simd_size; offset <<= 1) {
        float neighbor = simd_shuffle_up(val, offset);
        if (lane >= offset) {
            val += neighbor;
        }
    }

    // Convert inclusive scan to exclusive scan
    float exclusive = simd_shuffle_up(val, 1);
    exclusive = (lane == 0) ? 0.0f : exclusive;

    if (tid < count) {
        data[tid] = exclusive;
    }
}

// ---------------------------------------------------------------------------
// 4. Histogram — atomic_fetch_add on a 256-bin histogram
// ---------------------------------------------------------------------------
kernel void compute_histogram(
    device const uchar*  input  [[buffer(0)]],
    device atomic_uint*  bins   [[buffer(1)]],
    constant uint&       count  [[buffer(2)]],
    uint tid [[thread_position_in_grid]]
) {
    if (tid >= count) return;

    // Each thread processes 4 consecutive elements for throughput
    uint base = tid * 4;
    for (uint i = 0; i < 4 && (base + i) < count; ++i) {
        uint bin = uint(input[base + i]);
        atomic_fetch_add_explicit(&bins[bin], 1u, memory_order_relaxed);
    }
}

// ---------------------------------------------------------------------------
// 5. Image convolution — 3x3 kernel on texture2d
// ---------------------------------------------------------------------------
kernel void convolve_3x3(
    texture2d<float, access::read>  inTex  [[texture(0)]],
    texture2d<float, access::write> outTex [[texture(1)]],
    constant float*                 weights [[buffer(0)]],
    uint2 gid [[thread_position_in_grid]]
) {
    uint w = inTex.get_width();
    uint h = inTex.get_height();
    if (gid.x >= w || gid.y >= h) return;

    float4 acc = float4(0.0);
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            uint2 coord = uint2(
                clamp(int(gid.x) + dx, 0, int(w) - 1),
                clamp(int(gid.y) + dy, 0, int(h) - 1)
            );
            float weight = weights[(dy + 1) * 3 + (dx + 1)];
            acc += inTex.read(coord) * weight;
        }
    }
    outTex.write(acc, gid);
}

// ---------------------------------------------------------------------------
// 6. Bitonic sort step — one compare-and-swap pass
// ---------------------------------------------------------------------------
kernel void bitonic_sort_step(
    device float*   data      [[buffer(0)]],
    constant uint&  blockSize [[buffer(1)]],
    constant uint&  subSize   [[buffer(2)]],
    uint tid [[thread_position_in_grid]]
) {
    uint halfSub = subSize / 2;
    uint blockId = tid / halfSub;
    uint offset  = tid % halfSub;

    uint left  = blockId * subSize + offset;
    uint right = left + halfSub;

    // Determine sort direction: ascending for even blocks, descending for odd
    bool ascending = ((left / blockSize) % 2) == 0;

    float lv = data[left];
    float rv = data[right];

    if ((lv > rv) == ascending) {
        data[left]  = rv;
        data[right] = lv;
    }
}

// ---------------------------------------------------------------------------
// 7. N-body gravitational simulation — compute forces with float3 math
// ---------------------------------------------------------------------------
struct Particle {
    float3 position;
    float  mass;
    float3 velocity;
    float  _pad;
};

kernel void nbody_forces(
    device const Particle* particles [[buffer(0)]],
    device float3*         forces    [[buffer(1)]],
    constant uint&         numBodies [[buffer(2)]],
    uint tid [[thread_position_in_grid]]
) {
    if (tid >= numBodies) return;

    float3 pos = particles[tid].position;
    float3 force = float3(0.0);
    const float softening = 0.001f;
    const float G = 6.674e-11f;

    for (uint j = 0; j < numBodies; ++j) {
        if (j == tid) continue;

        float3 r = particles[j].position - pos;
        float distSq = dot(r, r) + softening;
        float invDist = rsqrt(distSq);
        float invDist3 = invDist * invDist * invDist;

        force += r * (G * particles[j].mass * invDist3);
    }

    forces[tid] = force;
}

// ---------------------------------------------------------------------------
// 8. Cooperative matrix multiply — simdgroup_float8x8 for small matmul
// ---------------------------------------------------------------------------
kernel void simdgroup_matmul(
    device const float* A [[buffer(0)]],
    device const float* B [[buffer(1)]],
    device float*       C [[buffer(2)]],
    constant uint&      M [[buffer(3)]],
    constant uint&      N [[buffer(4)]],
    constant uint&      K [[buffer(5)]],
    uint2 gid  [[thread_position_in_grid]],
    uint  lane [[thread_index_in_simdgroup]]
) {
    // Each simdgroup computes an 8x8 output tile
    simdgroup_float8x8 accum;
    simdgroup_float8x8 a_tile;
    simdgroup_float8x8 b_tile;

    // Zero-initialize accumulator
    accum = simdgroup_float8x8(0);

    uint baseRow = (gid.y / 8) * 8;
    uint baseCol = (gid.x / 8) * 8;

    // Accumulate over K dimension in 8-wide steps
    for (uint kk = 0; kk < K; kk += 8) {
        simdgroup_load(a_tile, A + baseRow * K + kk, K);
        simdgroup_load(b_tile, B + kk * N + baseCol, N);
        simdgroup_multiply_accumulate(accum, a_tile, b_tile, accum);
    }

    simdgroup_store(accum, C + baseRow * N + baseCol, N);
}
