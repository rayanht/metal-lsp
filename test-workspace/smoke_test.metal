#include <metal_stdlib>
using namespace metal;

// Compute kernel — basic array operation
kernel void add_arrays(
    device const float* inA [[buffer(0)]],
    device const float* inB [[buffer(1)]],
    device float* result [[buffer(2)]],
    uint index [[thread_position_in_grid]]
) {
    result[index] = inA[index] + inB[index];
}

// Texture sampling in a fragment shader
struct VertexOut {
    float4 position [[position]];
    float2 texCoord;
};

fragment float4 fragment_main(
    VertexOut in [[stage_in]],
    texture2d<float, access::sample> colorTexture [[texture(0)]],
    sampler textureSampler [[sampler(0)]]
) {
    float4 color = colorTexture.sample(textureSampler, in.texCoord);
    return color;
}

// Vertex shader with matrix transform
struct Uniforms {
    float4x4 modelViewProjection;
};

struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
};

vertex VertexOut vertex_main(
    VertexIn in [[stage_in]],
    constant Uniforms& uniforms [[buffer(0)]]
) {
    VertexOut out;
    out.position = uniforms.modelViewProjection * float4(in.position, 1.0);
    out.texCoord = in.texCoord;
    return out;
}

// Compute kernel with atomics and threadgroup memory
kernel void histogram(
    texture2d<float, access::read> input [[texture(0)]],
    device atomic_uint* bins [[buffer(0)]],
    uint2 gid [[thread_position_in_grid]]
) {
    float4 pixel = input.read(gid);
    float luminance = dot(pixel.rgb, float3(0.299, 0.587, 0.114));
    uint bin = clamp(uint(luminance * 255.0), 0u, 255u);
    atomic_fetch_add_explicit(&bins[bin], 1u, memory_order_relaxed);
}

// SIMD group operations
kernel void simd_reduce(
    device const float* input [[buffer(0)]],
    device float* output [[buffer(1)]],
    uint tid [[thread_position_in_grid]],
    uint simd_lane [[thread_index_in_simdgroup]],
    uint simd_group [[simdgroup_index_in_threadgroup]]
) {
    float val = input[tid];
    float sum = simd_sum(val);
    if (simd_lane == 0) {
        output[simd_group] = sum;
    }
}

// Half precision math
kernel void half_precision(
    device const half4* input [[buffer(0)]],
    device half4* output [[buffer(1)]],
    uint tid [[thread_position_in_grid]]
) {
    half4 v = input[tid];
    half4 result = normalize(v) * half(2.0h);
    result = clamp(result, half4(0.0h), half4(1.0h));
    output[tid] = result;
}

// Matrix operations
kernel void matrix_multiply(
    device const float4x4* A [[buffer(0)]],
    device const float4x4* B [[buffer(1)]],
    device float4x4* C [[buffer(2)]],
    uint tid [[thread_position_in_grid]]
) {
    C[tid] = A[tid] * B[tid];
}
