#pragma once
#include <metal_types.h>

namespace metal {

// ============================================================================
// Geometric functions — operate on float and half vector types
// ============================================================================

/// Compute the cross product of two 3-component vectors
float3 cross(float3 x, float3 y);
half3 cross(half3 x, half3 y);

/// Compute the distance between two points
float distance(float x, float y);
float distance(float2 x, float2 y);
float distance(float3 x, float3 y);
float distance(float4 x, float4 y);
half distance(half x, half y);
half distance(half2 x, half2 y);
half distance(half3 x, half3 y);
half distance(half4 x, half4 y);

/// Compute the dot product
float dot(float x, float y);
float dot(float2 x, float2 y);
float dot(float3 x, float3 y);
float dot(float4 x, float4 y);
half dot(half x, half y);
half dot(half2 x, half2 y);
half dot(half3 x, half3 y);
half dot(half4 x, half4 y);

/// Return n if dot(nref, i) < 0, else return -n
float faceforward(float n, float i, float nref);
float2 faceforward(float2 n, float2 i, float2 nref);
float3 faceforward(float3 n, float3 i, float3 nref);
float4 faceforward(float4 n, float4 i, float4 nref);
half faceforward(half n, half i, half nref);
half2 faceforward(half2 n, half2 i, half2 nref);
half3 faceforward(half3 n, half3 i, half3 nref);
half4 faceforward(half4 n, half4 i, half4 nref);

/// Compute the length (magnitude) of a vector
float length(float x);
float length(float2 x);
float length(float3 x);
float length(float4 x);
half length(half x);
half length(half2 x);
half length(half3 x);
half length(half4 x);

/// Normalize a vector to unit length
float normalize(float x);
float2 normalize(float2 x);
float3 normalize(float3 x);
float4 normalize(float4 x);
half normalize(half x);
half2 normalize(half2 x);
half3 normalize(half3 x);
half4 normalize(half4 x);

/// Compute the reflection direction: i - 2*dot(n,i)*n
float reflect(float i, float n);
float2 reflect(float2 i, float2 n);
float3 reflect(float3 i, float3 n);
float4 reflect(float4 i, float4 n);
half reflect(half i, half n);
half2 reflect(half2 i, half2 n);
half3 reflect(half3 i, half3 n);
half4 reflect(half4 i, half4 n);

/// Compute the refraction direction given incidence vector, normal, and index of refraction ratio
float refract(float i, float n, float eta);
float2 refract(float2 i, float2 n, float eta);
float3 refract(float3 i, float3 n, float eta);
float4 refract(float4 i, float4 n, float eta);
half refract(half i, half n, half eta);
half2 refract(half2 i, half2 n, half eta);
half3 refract(half3 i, half3 n, half eta);
half4 refract(half4 i, half4 n, half eta);

/// Compute the squared distance between two points
float distance_squared(float x, float y);
float distance_squared(float2 x, float2 y);
float distance_squared(float3 x, float3 y);
float distance_squared(float4 x, float4 y);
half distance_squared(half x, half y);
half distance_squared(half2 x, half2 y);
half distance_squared(half3 x, half3 y);
half distance_squared(half4 x, half4 y);

/// Compute the squared length of a vector
float length_squared(float x);
float length_squared(float2 x);
float length_squared(float3 x);
float length_squared(float4 x);
half length_squared(half x);
half length_squared(half2 x);
half length_squared(half3 x);
half length_squared(half4 x);

} // namespace metal
