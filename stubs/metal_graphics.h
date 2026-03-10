#pragma once
#include <metal_types.h>

namespace metal {

// ============================================================================
// Derivative functions — fragment shader screen-space derivatives
// ============================================================================

/// Compute the partial derivative of x with respect to screen-space x
float dfdx(float x);
float2 dfdx(float2 x);
float3 dfdx(float3 x);
float4 dfdx(float4 x);
half dfdx(half x);
half2 dfdx(half2 x);
half3 dfdx(half3 x);
half4 dfdx(half4 x);

/// Compute the partial derivative of x with respect to screen-space y
float dfdy(float x);
float2 dfdy(float2 x);
float3 dfdy(float3 x);
float4 dfdy(float4 x);
half dfdy(half x);
half2 dfdy(half2 x);
half3 dfdy(half3 x);
half4 dfdy(half4 x);

/// Compute the sum of absolute derivatives: abs(dfdx(x)) + abs(dfdy(x))
float fwidth(float x);
float2 fwidth(float2 x);
float3 fwidth(float3 x);
float4 fwidth(float4 x);
half fwidth(half x);
half2 fwidth(half2 x);
half3 fwidth(half3 x);
half4 fwidth(half4 x);

// ============================================================================
// Coarse derivative variants
// ============================================================================

/// Compute the coarse partial derivative with respect to screen-space x
float dfdx_coarse(float x);
float2 dfdx_coarse(float2 x);
float3 dfdx_coarse(float3 x);
float4 dfdx_coarse(float4 x);
half dfdx_coarse(half x);
half2 dfdx_coarse(half2 x);
half3 dfdx_coarse(half3 x);
half4 dfdx_coarse(half4 x);

/// Compute the coarse partial derivative with respect to screen-space y
float dfdy_coarse(float x);
float2 dfdy_coarse(float2 x);
float3 dfdy_coarse(float3 x);
float4 dfdy_coarse(float4 x);
half dfdy_coarse(half x);
half2 dfdy_coarse(half2 x);
half3 dfdy_coarse(half3 x);
half4 dfdy_coarse(half4 x);

/// Compute the coarse sum of absolute derivatives
float fwidth_coarse(float x);
float2 fwidth_coarse(float2 x);
float3 fwidth_coarse(float3 x);
float4 fwidth_coarse(float4 x);
half fwidth_coarse(half x);
half2 fwidth_coarse(half2 x);
half3 fwidth_coarse(half3 x);
half4 fwidth_coarse(half4 x);

// ============================================================================
// Fine derivative variants
// ============================================================================

/// Compute the fine partial derivative with respect to screen-space x
float dfdx_fine(float x);
float2 dfdx_fine(float2 x);
float3 dfdx_fine(float3 x);
float4 dfdx_fine(float4 x);
half dfdx_fine(half x);
half2 dfdx_fine(half2 x);
half3 dfdx_fine(half3 x);
half4 dfdx_fine(half4 x);

/// Compute the fine partial derivative with respect to screen-space y
float dfdy_fine(float x);
float2 dfdy_fine(float2 x);
float3 dfdy_fine(float3 x);
float4 dfdy_fine(float4 x);
half dfdy_fine(half x);
half2 dfdy_fine(half2 x);
half3 dfdy_fine(half3 x);
half4 dfdy_fine(half4 x);

/// Compute the fine sum of absolute derivatives
float fwidth_fine(float x);
float2 fwidth_fine(float2 x);
float3 fwidth_fine(float3 x);
float4 fwidth_fine(float4 x);
half fwidth_fine(half x);
half2 fwidth_fine(half2 x);
half3 fwidth_fine(half3 x);
half4 fwidth_fine(half4 x);

// ============================================================================
// Interpolation functions — advanced fragment shader interpolation
// ============================================================================

/// Interpolate the value at the pixel center
float interpolate_at_center(float x);
float2 interpolate_at_center(float2 x);
float3 interpolate_at_center(float3 x);
float4 interpolate_at_center(float4 x);
half interpolate_at_center(half x);
half2 interpolate_at_center(half2 x);
half3 interpolate_at_center(half3 x);
half4 interpolate_at_center(half4 x);

/// Interpolate the value at the centroid
float interpolate_at_centroid(float x);
float2 interpolate_at_centroid(float2 x);
float3 interpolate_at_centroid(float3 x);
float4 interpolate_at_centroid(float4 x);
half interpolate_at_centroid(half x);
half2 interpolate_at_centroid(half2 x);
half3 interpolate_at_centroid(half3 x);
half4 interpolate_at_centroid(half4 x);

/// Interpolate the value at a specific sample index
float interpolate_at_sample(float x, uint sample);
float2 interpolate_at_sample(float2 x, uint sample);
float3 interpolate_at_sample(float3 x, uint sample);
float4 interpolate_at_sample(float4 x, uint sample);
half interpolate_at_sample(half x, uint sample);
half2 interpolate_at_sample(half2 x, uint sample);
half3 interpolate_at_sample(half3 x, uint sample);
half4 interpolate_at_sample(half4 x, uint sample);

/// Interpolate the value at a given offset from the pixel center
float interpolate_at_offset(float x, float2 offset);
float2 interpolate_at_offset(float2 x, float2 offset);
float3 interpolate_at_offset(float3 x, float2 offset);
float4 interpolate_at_offset(float4 x, float2 offset);
half interpolate_at_offset(half x, float2 offset);
half2 interpolate_at_offset(half2 x, float2 offset);
half3 interpolate_at_offset(half3 x, float2 offset);
half4 interpolate_at_offset(half4 x, float2 offset);

// ============================================================================
// Fragment-specific functions
// ============================================================================

/// Discard the current fragment
void discard_fragment();

// ============================================================================
// Render target functions — tile-based deferred rendering
// ============================================================================

/// Read color attachment at given index for the current fragment position
template <typename T>
T render_target_read(uint color_attachment_index);

/// Read color attachment at given sample index (MSAA)
template <typename T>
T render_target_read(uint color_attachment_index, uint sample_index);

/// Write to color attachment at given index for the current fragment position
template <typename T>
void render_target_write(T value, uint color_attachment_index);

/// Write to color attachment at given sample index (MSAA)
template <typename T>
void render_target_write(T value, uint color_attachment_index, uint sample_index);

/// Return the number of samples for the render target
uint get_render_target_sample_count();

/// Return the fragment size in pixels (used for variable rasterization rate)
ushort2 get_fragment_size();

/// Return the number of color attachments
uint get_num_color_attachments();

// ============================================================================
// Fragment position and front-facing
// ============================================================================

/// Return true if the current fragment is front-facing
bool is_front_face();

} // namespace metal
