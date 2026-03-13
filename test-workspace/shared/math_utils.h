#pragma once

inline float3 srgb_to_linear(float3 c) {
    return pow(c, 2.2f);
}

inline float3 linear_to_srgb(float3 c) {
    return pow(c, 1.0f / 2.2f);
}

inline float luminance(float3 c) {
    return dot(c, float3(0.2126f, 0.7152f, 0.0722f));
}

inline float3 tonemap_aces(float3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}
