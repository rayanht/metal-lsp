#include <metal_stdlib>
using namespace metal;

kernel void add_arrays(
    device const float* inA [[buffer(0)]],
    device const float* inB [[buffer(1)]],
    device float* result [[buffer(2)]],
    uint index [[thread_position_in_grid]]
) {
    result[index] = inA[index] + inB[index];
}
