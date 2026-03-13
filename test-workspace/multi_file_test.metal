#include <metal_stdlib>
using namespace metal;

// These includes resolve because workspace root is on the include path
#include "shared/constants.h"
#include "shared/math_utils.h"

// Forward lighting kernel using shared types and utilities
kernel void forward_lighting(
    device float4* output [[buffer(0)]],
    const device Light* lights [[buffer(1)]],
    const device float3* positions [[buffer(2)]],
    const device float3* normals [[buffer(3)]],
    constant uint& numLights [[buffer(4)]],
    uint tid [[thread_position_in_grid]]
) {
    float3 pos = positions[tid];
    float3 N = normalize(normals[tid]);
    float3 color = float3(0.0f);

    for (uint i = 0; i < min(numLights, MAX_LIGHTS); i++) {
        float3 L = lights[i].position - pos;
        float dist = length(L);
        L /= dist;

        float attenuation = saturate(1.0f - dist / lights[i].radius);
        float NdotL = max(dot(N, L), 0.0f);

        color += lights[i].color * lights[i].intensity * NdotL * attenuation;
    }

    // Apply tonemapping from shared utils
    color = tonemap_aces(color);
    color = linear_to_srgb(color);

    output[tid] = float4(color, 1.0f);
}
