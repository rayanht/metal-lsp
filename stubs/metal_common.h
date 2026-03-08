#pragma once
#include <metal_types.h>

namespace metal {

// ============================================================================
// Common functions — overloads for scalar and vector types
// These mirror the Metal Shading Language spec section 6.2
// ============================================================================

/// Clamp x to the range [minval, maxval]
float clamp(float x, float minval, float maxval);
float2 clamp(float2 x, float2 minval, float2 maxval);
float3 clamp(float3 x, float3 minval, float3 maxval);
float4 clamp(float4 x, float4 minval, float4 maxval);
half clamp(half x, half minval, half maxval);
half2 clamp(half2 x, half2 minval, half2 maxval);
half3 clamp(half3 x, half3 minval, half3 maxval);
half4 clamp(half4 x, half4 minval, half4 maxval);
int clamp(int x, int minval, int maxval);
uint clamp(uint x, uint minval, uint maxval);

/// Linear interpolation: returns x + t*(y - x)
float mix(float x, float y, float t);
float2 mix(float2 x, float2 y, float2 t);
float3 mix(float3 x, float3 y, float3 t);
float4 mix(float4 x, float4 y, float4 t);
half mix(half x, half y, half t);
half2 mix(half2 x, half2 y, half2 t);
half3 mix(half3 x, half3 y, half3 t);
half4 mix(half4 x, half4 y, half4 t);
/// Mix with scalar t applied to each component
float2 mix(float2 x, float2 y, float t);
float3 mix(float3 x, float3 y, float t);
float4 mix(float4 x, float4 y, float t);
half2 mix(half2 x, half2 y, half t);
half3 mix(half3 x, half3 y, half t);
half4 mix(half4 x, half4 y, half t);

/// Clamp x to [0.0, 1.0]
float saturate(float x);
float2 saturate(float2 x);
float3 saturate(float3 x);
float4 saturate(float4 x);
half saturate(half x);
half2 saturate(half2 x);
half3 saturate(half3 x);
half4 saturate(half4 x);

/// Return 0.0 if x < edge, else 1.0
float step(float edge, float x);
float2 step(float2 edge, float2 x);
float3 step(float3 edge, float3 x);
float4 step(float4 edge, float4 x);
half step(half edge, half x);
half2 step(half2 edge, half2 x);
half3 step(half3 edge, half3 x);
half4 step(half4 edge, half4 x);

/// Hermite interpolation between 0 and 1 when edge0 < x < edge1
float smoothstep(float edge0, float edge1, float x);
float2 smoothstep(float2 edge0, float2 edge1, float2 x);
float3 smoothstep(float3 edge0, float3 edge1, float3 x);
float4 smoothstep(float4 edge0, float4 edge1, float4 x);
half smoothstep(half edge0, half edge1, half x);
half2 smoothstep(half2 edge0, half2 edge1, half2 x);
half3 smoothstep(half3 edge0, half3 edge1, half3 x);
half4 smoothstep(half4 edge0, half4 edge1, half4 x);

/// Return the sign of x: -1.0, 0.0, or 1.0
float sign(float x);
float2 sign(float2 x);
float3 sign(float3 x);
float4 sign(float4 x);
half sign(half x);
half2 sign(half2 x);
half3 sign(half3 x);
half4 sign(half4 x);

/// Return the maximum of x and y
float max(float x, float y);
float2 max(float2 x, float2 y);
float3 max(float3 x, float3 y);
float4 max(float4 x, float4 y);
half max(half x, half y);
half2 max(half2 x, half2 y);
half3 max(half3 x, half3 y);
half4 max(half4 x, half4 y);

/// Return the minimum of x and y
float min(float x, float y);
float2 min(float2 x, float2 y);
float3 min(float3 x, float3 y);
float4 min(float4 x, float4 y);
half min(half x, half y);
half2 min(half2 x, half2 y);
half3 min(half3 x, half3 y);
half4 min(half4 x, half4 y);

/// Absolute value
float abs(float x);
float2 abs(float2 x);
float3 abs(float3 x);
float4 abs(float4 x);
half abs(half x);
half2 abs(half2 x);
half3 abs(half3 x);
half4 abs(half4 x);
int abs(int x);
int2 abs(int2 x);
int3 abs(int3 x);
int4 abs(int4 x);

} // namespace metal
