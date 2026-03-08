#pragma once
#include <metal_types.h>

namespace metal {

// ============================================================================
// Trigonometric functions
// ============================================================================

/// Compute arc cosine
template <typename T> T acos(T x);
/// Compute arc sine
template <typename T> T asin(T x);
/// Compute arc tangent
template <typename T> T atan(T x);
/// Compute arc tangent of y/x
template <typename T> T atan2(T y, T x);
/// Compute cosine
template <typename T> T cos(T x);
/// Compute hyperbolic cosine
template <typename T> T cosh(T x);
/// Compute sine
template <typename T> T sin(T x);
/// Compute sine and cosine simultaneously
template <typename T> T sincos(T x, T& cosval);
/// Compute hyperbolic sine
template <typename T> T sinh(T x);
/// Compute tangent
template <typename T> T tan(T x);
/// Compute hyperbolic tangent
template <typename T> T tanh(T x);

// Scalar overloads
float acos(float x);
float asin(float x);
float atan(float x);
float atan2(float y, float x);
float cos(float x);
float cosh(float x);
float sin(float x);
float sinh(float x);
float tan(float x);
float tanh(float x);
half acos(half x);
half asin(half x);
half atan(half x);
half atan2(half y, half x);
half cos(half x);
half cosh(half x);
half sin(half x);
half sinh(half x);
half tan(half x);
half tanh(half x);

// ============================================================================
// Exponential and logarithmic functions
// ============================================================================

/// Compute e^x
template <typename T> T exp(T x);
/// Compute 2^x
template <typename T> T exp2(T x);
/// Compute natural logarithm
template <typename T> T log(T x);
/// Compute base-2 logarithm
template <typename T> T log2(T x);
/// Compute base-10 logarithm
template <typename T> T log10(T x);

float exp(float x);
float exp2(float x);
float log(float x);
float log2(float x);
float log10(float x);
half exp(half x);
half exp2(half x);
half log(half x);
half log2(half x);
half log10(half x);

// ============================================================================
// Power functions
// ============================================================================

/// Compute x raised to the power y
template <typename T> T pow(T x, T y);
/// Compute x * 2^n efficiently
template <typename T> T ldexp(T x, int n);
/// Compute inverse square root (1/sqrt(x))
template <typename T> T rsqrt(T x);
/// Compute square root
template <typename T> T sqrt(T x);

float pow(float x, float y);
float ldexp(float x, int n);
float rsqrt(float x);
float sqrt(float x);
half pow(half x, half y);
half rsqrt(half x);
half sqrt(half x);

// ============================================================================
// Rounding functions
// ============================================================================

/// Round to smallest integral value not less than x
template <typename T> T ceil(T x);
/// Round to largest integral value not greater than x
template <typename T> T floor(T x);
/// Round to nearest integral value
template <typename T> T round(T x);
/// Round toward zero
template <typename T> T trunc(T x);
/// Return fractional part of x
template <typename T> T fract(T x);

float ceil(float x);
float floor(float x);
float round(float x);
float trunc(float x);
float fract(float x);
half ceil(half x);
half floor(half x);
half round(half x);
half trunc(half x);
half fract(half x);

// ============================================================================
// Common math functions
// ============================================================================

/// Compute absolute value
template <typename T> T abs(T x);
/// Compute fused multiply-add: a*b + c
template <typename T> T fma(T a, T b, T c);
/// Compute floating-point remainder of x/y
template <typename T> T fmod(T x, T y);
/// Return maximum of x and y
template <typename T> T fmax(T x, T y);
/// Return minimum of x and y
template <typename T> T fmin(T x, T y);
/// Compute absolute value (floating-point)
template <typename T> T fabs(T x);
/// Split x into fractional and integral parts
template <typename T> T modf(T x, T& intpart);
/// Extract mantissa and exponent
template <typename T> T frexp(T x, int& exp);

float abs(float x);
float fma(float a, float b, float c);
float fmod(float x, float y);
float fmax(float x, float y);
float fmin(float x, float y);
float fabs(float x);
half abs(half x);
half fma(half a, half b, half c);
half fmod(half x, half y);
half fmax(half x, half y);
half fmin(half x, half y);
half fabs(half x);
int abs(int x);
short abs(short x);

/// Return the maximum of x and y
template <typename T> T max(T x, T y);
/// Return the minimum of x and y
template <typename T> T min(T x, T y);
/// Clamp x to the range [minval, maxval]
template <typename T> T clamp(T x, T minval, T maxval);

float max(float x, float y);
float min(float x, float y);
float clamp(float x, float minval, float maxval);
half max(half x, half y);
half min(half x, half y);
half clamp(half x, half minval, half maxval);
int max(int x, int y);
int min(int x, int y);
int clamp(int x, int minval, int maxval);
uint max(uint x, uint y);
uint min(uint x, uint y);
uint clamp(uint x, uint minval, uint maxval);
short max(short x, short y);
short min(short x, short y);
ushort max(ushort x, ushort y);
ushort min(ushort x, ushort y);

/// Linear interpolation: x + t*(y - x)
template <typename T> T mix(T x, T y, T t);
/// Clamp x to [0.0, 1.0]
template <typename T> T saturate(T x);
/// Return 0.0 if x < edge, else 1.0
template <typename T> T step(T edge, T x);
/// Hermite interpolation between 0 and 1
template <typename T> T smoothstep(T edge0, T edge1, T x);
/// Return sign of x (-1, 0, or 1)
template <typename T> T sign(T x);

float mix(float x, float y, float t);
float saturate(float x);
float step(float edge, float x);
float smoothstep(float edge0, float edge1, float x);
float sign(float x);
half mix(half x, half y, half t);
half saturate(half x);
half step(half edge, half x);
half smoothstep(half edge0, half edge1, half x);
half sign(half x);

/// Convert radians to degrees
template <typename T> T degrees(T radians);
/// Convert degrees to radians
template <typename T> T radians(T degrees);

float degrees(float radians);
float radians(float degrees);
half degrees(half radians);
half radians(half degrees);

// ============================================================================
// Comparison / classification functions
// ============================================================================

/// Return true if x is infinity
template <typename T> bool isinf(T x);
/// Return true if x is NaN
template <typename T> bool isnan(T x);

bool isinf(float x);
bool isnan(float x);
bool isinf(half x);
bool isnan(half x);

// ============================================================================
// Vector overloads for common math functions
// clangd can't deduce template T when mixing vecN<T> and T args, so we need
// explicit overloads for all vector sizes.
// ============================================================================

#define _METAL_MATH_UNARY_VEC(fn) \
    float2 fn(float2 x); \
    float3 fn(float3 x); \
    float4 fn(float4 x); \
    half2 fn(half2 x); \
    half3 fn(half3 x); \
    half4 fn(half4 x);

#define _METAL_MATH_BINARY_VEC(fn) \
    float2 fn(float2 x, float2 y); \
    float3 fn(float3 x, float3 y); \
    float4 fn(float4 x, float4 y); \
    half2 fn(half2 x, half2 y); \
    half3 fn(half3 x, half3 y); \
    half4 fn(half4 x, half4 y);

#define _METAL_MATH_TERNARY_VEC(fn) \
    float2 fn(float2 a, float2 b, float2 c); \
    float3 fn(float3 a, float3 b, float3 c); \
    float4 fn(float4 a, float4 b, float4 c); \
    half2 fn(half2 a, half2 b, half2 c); \
    half3 fn(half3 a, half3 b, half3 c); \
    half4 fn(half4 a, half4 b, half4 c);

_METAL_MATH_UNARY_VEC(abs)
_METAL_MATH_UNARY_VEC(acos)
_METAL_MATH_UNARY_VEC(asin)
_METAL_MATH_UNARY_VEC(atan)
_METAL_MATH_UNARY_VEC(ceil)
_METAL_MATH_UNARY_VEC(cos)
_METAL_MATH_UNARY_VEC(cosh)
_METAL_MATH_UNARY_VEC(exp)
_METAL_MATH_UNARY_VEC(exp2)
_METAL_MATH_UNARY_VEC(fabs)
_METAL_MATH_UNARY_VEC(floor)
_METAL_MATH_UNARY_VEC(fract)
_METAL_MATH_UNARY_VEC(log)
_METAL_MATH_UNARY_VEC(log2)
_METAL_MATH_UNARY_VEC(log10)
_METAL_MATH_UNARY_VEC(round)
_METAL_MATH_UNARY_VEC(rsqrt)
_METAL_MATH_UNARY_VEC(saturate)
_METAL_MATH_UNARY_VEC(sign)
_METAL_MATH_UNARY_VEC(sin)
_METAL_MATH_UNARY_VEC(sinh)
_METAL_MATH_UNARY_VEC(sqrt)
_METAL_MATH_UNARY_VEC(tan)
_METAL_MATH_UNARY_VEC(tanh)
_METAL_MATH_UNARY_VEC(trunc)

_METAL_MATH_BINARY_VEC(atan2)
_METAL_MATH_BINARY_VEC(fmax)
_METAL_MATH_BINARY_VEC(fmin)
_METAL_MATH_BINARY_VEC(fmod)
_METAL_MATH_BINARY_VEC(max)
_METAL_MATH_BINARY_VEC(min)
_METAL_MATH_BINARY_VEC(pow)
_METAL_MATH_BINARY_VEC(step)

_METAL_MATH_TERNARY_VEC(clamp)
_METAL_MATH_TERNARY_VEC(fma)
_METAL_MATH_TERNARY_VEC(mix)
_METAL_MATH_TERNARY_VEC(smoothstep)

// Mixed scalar/vector clamp and mix
float2 clamp(float2 x, float lo, float hi);
float3 clamp(float3 x, float lo, float hi);
float4 clamp(float4 x, float lo, float hi);
half2 clamp(half2 x, half lo, half hi);
half3 clamp(half3 x, half lo, half hi);
half4 clamp(half4 x, half lo, half hi);

float2 mix(float2 x, float2 y, float a);
float3 mix(float3 x, float3 y, float a);
float4 mix(float4 x, float4 y, float a);
half2 mix(half2 x, half2 y, half a);
half3 mix(half3 x, half3 y, half a);
half4 mix(half4 x, half4 y, half a);

// fma with mixed scalar/vector
float2 fma(float2 a, float b, float2 c);
float3 fma(float3 a, float b, float3 c);
float4 fma(float4 a, float b, float4 c);
float2 fma(float2 a, float2 b, float c);
float3 fma(float3 a, float3 b, float c);
float4 fma(float4 a, float4 b, float c);

#undef _METAL_MATH_UNARY_VEC
#undef _METAL_MATH_BINARY_VEC
#undef _METAL_MATH_TERNARY_VEC

} // namespace metal
