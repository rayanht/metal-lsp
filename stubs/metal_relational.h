#pragma once
#include <metal_types.h>

namespace metal {

// ============================================================================
// Relational functions
// ============================================================================

/// Return true if all components of x are true (non-zero)
bool all(bool2 x);
bool all(bool3 x);
bool all(bool4 x);
bool all(bool x);

/// Return true if any component of x is true (non-zero)
bool any(bool2 x);
bool any(bool3 x);
bool any(bool4 x);
bool any(bool x);

/// Return true if x is a finite value (not NaN, not infinity)
bool isfinite(float x);
bool2 isfinite(float2 x);
bool3 isfinite(float3 x);
bool4 isfinite(float4 x);
bool isfinite(half x);
bool2 isfinite(half2 x);
bool3 isfinite(half3 x);
bool4 isfinite(half4 x);

/// Return true if x is positive or negative infinity
bool isinf(float x);
bool2 isinf(float2 x);
bool3 isinf(float3 x);
bool4 isinf(float4 x);
bool isinf(half x);
bool2 isinf(half2 x);
bool3 isinf(half3 x);
bool4 isinf(half4 x);

/// Return true if x is NaN
bool isnan(float x);
bool2 isnan(float2 x);
bool3 isnan(float3 x);
bool4 isnan(float4 x);
bool isnan(half x);
bool2 isnan(half2 x);
bool3 isnan(half3 x);
bool4 isnan(half4 x);

/// Return true if x is a normal value (not zero, subnormal, infinity, or NaN)
bool isnormal(float x);
bool2 isnormal(float2 x);
bool3 isnormal(float3 x);
bool4 isnormal(float4 x);
bool isnormal(half x);
bool2 isnormal(half2 x);
bool3 isnormal(half3 x);
bool4 isnormal(half4 x);

/// Return true if x or y is NaN (unordered comparison)
bool isunordered(float x, float y);
bool2 isunordered(float2 x, float2 y);
bool3 isunordered(float3 x, float3 y);
bool4 isunordered(float4 x, float4 y);
bool isunordered(half x, half y);
bool2 isunordered(half2 x, half2 y);
bool3 isunordered(half3 x, half3 y);
bool4 isunordered(half4 x, half4 y);

/// Component-wise selection: for each component, return b[i] if c[i] is true, else a[i]
template <typename T>
T select(T a, T b, bool c);

float select(float a, float b, bool c);
float2 select(float2 a, float2 b, bool2 c);
float3 select(float3 a, float3 b, bool3 c);
float4 select(float4 a, float4 b, bool4 c);
half select(half a, half b, bool c);
half2 select(half2 a, half2 b, bool2 c);
half3 select(half3 a, half3 b, bool3 c);
half4 select(half4 a, half4 b, bool4 c);
int select(int a, int b, bool c);
int2 select(int2 a, int2 b, bool2 c);
int3 select(int3 a, int3 b, bool3 c);
int4 select(int4 a, int4 b, bool4 c);
uint select(uint a, uint b, bool c);
uint2 select(uint2 a, uint2 b, bool2 c);
uint3 select(uint3 a, uint3 b, bool3 c);
uint4 select(uint4 a, uint4 b, bool4 c);

} // namespace metal
