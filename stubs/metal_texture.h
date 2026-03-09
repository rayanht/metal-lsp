#pragma once
#include <metal_types.h>

namespace metal {

// Forward declare sampler
struct sampler;

/// LOD level for texture sampling
struct level {
    float lod;
    /// Construct mip level
    level(float l) : lod(l) {}
};

/// Gradient for 2D texture sampling
struct gradient2d {
    float2 dPdx, dPdy;
    gradient2d(float2 dx, float2 dy) : dPdx(dx), dPdy(dy) {}
};

/// Gradient for 3D texture sampling
struct gradient3d {
    float3 dPdx, dPdy, dPdz;
    gradient3d(float3 dx, float3 dy, float3 dz) : dPdx(dx), dPdy(dy), dPdz(dz) {}
};

/// Bias for texture sampling
struct bias {
    float value;
    bias(float v) : value(v) {}
};

/// Min LOD clamp for texture sampling
struct min_lod_clamp {
    float value;
    min_lod_clamp(float v) : value(v) {}
};

// ============================================================================
// texture1d
// ============================================================================

/// 1D texture type
template <typename T, access A = access::sample>
struct texture1d {
    /// Read a texel at coordinate x
    vec4<T> read(uint coord) const;
    /// Read a texel at coordinate x and mip level
    vec4<T> read(uint coord, uint lod) const;
    /// Write a color to coordinate x
    void write(vec4<T> color, uint coord);
    /// Sample the texture at normalized coordinate
    vec4<T> sample(sampler s, float coord) const;
    /// Return the texture width
    uint get_width(uint lod = 0) const;
    /// Return the number of mip levels
    uint get_num_mip_levels() const;
};

// ============================================================================
// texture2d
// ============================================================================

/// 2D texture type
template <typename T, access A = access::sample>
struct texture2d {
    /// Read a texel at coordinate
    vec4<T> read(uint2 coord) const;
    /// Read a texel at coordinate and mip level
    vec4<T> read(uint2 coord, uint lod) const;
    /// Write a color to coordinate
    void write(vec4<T> color, uint2 coord);
    /// Write a color to coordinate at mip level
    void write(vec4<T> color, uint2 coord, uint lod);
    /// Sample at normalized coordinate
    vec4<T> sample(sampler s, float2 coord) const;
    /// Sample at normalized coordinate with LOD level
    vec4<T> sample(sampler s, float2 coord, level lod) const;
    /// Sample at normalized coordinate with bias
    vec4<T> sample(sampler s, float2 coord, bias b) const;
    /// Sample at normalized coordinate with gradient
    vec4<T> sample(sampler s, float2 coord, gradient2d grad) const;
    /// Sample at normalized coordinate with min LOD clamp
    vec4<T> sample(sampler s, float2 coord, min_lod_clamp lod) const;
    /// Gather the four texels for bilinear filtering (component 0)
    vec4<T> gather(sampler s, float2 coord, int2 offset = int2(0)) const;
    /// Return the texture width
    uint get_width(uint lod = 0) const;
    /// Return the texture height
    uint get_height(uint lod = 0) const;
    /// Return the number of mip levels
    uint get_num_mip_levels() const;
};

// ============================================================================
// texture3d
// ============================================================================

/// 3D texture type
template <typename T, access A = access::sample>
struct texture3d {
    /// Read a texel at coordinate
    vec4<T> read(uint3 coord) const;
    /// Read a texel at coordinate and mip level
    vec4<T> read(uint3 coord, uint lod) const;
    /// Write a color to coordinate
    void write(vec4<T> color, uint3 coord);
    /// Sample at normalized coordinate
    vec4<T> sample(sampler s, float3 coord) const;
    /// Sample at normalized coordinate with LOD level
    vec4<T> sample(sampler s, float3 coord, level lod) const;
    /// Sample at normalized coordinate with gradient
    vec4<T> sample(sampler s, float3 coord, gradient3d grad) const;
    /// Return the texture width
    uint get_width(uint lod = 0) const;
    /// Return the texture height
    uint get_height(uint lod = 0) const;
    /// Return the texture depth
    uint get_depth(uint lod = 0) const;
    /// Return the number of mip levels
    uint get_num_mip_levels() const;
};

// ============================================================================
// texture2d_array
// ============================================================================

/// 2D array texture type
template <typename T, access A = access::sample>
struct texture2d_array {
    /// Read a texel at coordinate and array index
    vec4<T> read(uint2 coord, uint array) const;
    /// Read a texel at coordinate, array index, and mip level
    vec4<T> read(uint2 coord, uint array, uint lod) const;
    /// Write a color to coordinate and array index
    void write(vec4<T> color, uint2 coord, uint array);
    /// Sample at normalized coordinate and array index
    vec4<T> sample(sampler s, float2 coord, uint array) const;
    /// Sample at normalized coordinate, array index, with LOD
    vec4<T> sample(sampler s, float2 coord, uint array, level lod) const;
    /// Gather four texels for bilinear filtering
    vec4<T> gather(sampler s, float2 coord, uint array, int2 offset = int2(0)) const;
    /// Return the texture width
    uint get_width(uint lod = 0) const;
    /// Return the texture height
    uint get_height(uint lod = 0) const;
    /// Return the number of array elements
    uint get_array_size() const;
    /// Return the number of mip levels
    uint get_num_mip_levels() const;
};

// ============================================================================
// texturecube
// ============================================================================

/// Cube texture type
template <typename T, access A = access::sample>
struct texturecube {
    /// Read a texel at coordinate and face
    vec4<T> read(uint2 coord, uint face) const;
    /// Read a texel at coordinate, face, and mip level
    vec4<T> read(uint2 coord, uint face, uint lod) const;
    /// Write a color to coordinate and face
    void write(vec4<T> color, uint2 coord, uint face);
    /// Sample at direction vector
    vec4<T> sample(sampler s, float3 coord) const;
    /// Sample at direction vector with LOD
    vec4<T> sample(sampler s, float3 coord, level lod) const;
    /// Sample at direction vector with bias
    vec4<T> sample(sampler s, float3 coord, bias b) const;
    /// Gather four texels for bilinear filtering
    vec4<T> gather(sampler s, float3 coord) const;
    /// Return the texture width
    uint get_width(uint lod = 0) const;
    /// Return the texture height
    uint get_height(uint lod = 0) const;
    /// Return the number of mip levels
    uint get_num_mip_levels() const;
};

// ============================================================================
// texturecube_array
// ============================================================================

/// Cube array texture type
template <typename T, access A = access::sample>
struct texturecube_array {
    /// Sample at direction vector and array index
    vec4<T> sample(sampler s, float3 coord, uint array) const;
    /// Sample at direction vector, array index, with LOD
    vec4<T> sample(sampler s, float3 coord, uint array, level lod) const;
    /// Gather four texels
    vec4<T> gather(sampler s, float3 coord, uint array) const;
    /// Return the texture width
    uint get_width(uint lod = 0) const;
    /// Return the texture height
    uint get_height(uint lod = 0) const;
    /// Return the number of array elements
    uint get_array_size() const;
    /// Return the number of mip levels
    uint get_num_mip_levels() const;
};

// ============================================================================
// texture2d_ms (multisample)
// ============================================================================

/// 2D multisample texture type
template <typename T, access A = access::read>
struct texture2d_ms {
    /// Read a sample at coordinate
    vec4<T> read(uint2 coord, uint sample) const;
    /// Return the texture width
    uint get_width() const;
    /// Return the texture height
    uint get_height() const;
    /// Return the number of samples per pixel
    uint get_num_samples() const;
};

// ============================================================================
// texture2d_ms_array (multisample array)
// ============================================================================

/// Multisample 2D array texture
template <typename T, access A = access::sample>
struct texture2d_ms_array {
    /// Read a texel at the given coordinates, array index, and sample
    T read(uint2 coord, uint array, uint sample) const;
    /// Get the texture width
    uint get_width() const;
    /// Get the texture height
    uint get_height() const;
    /// Get the number of array elements
    uint get_array_size() const;
    /// Get the number of samples per pixel
    uint get_num_samples() const;
};

// ============================================================================
// depth2d
// ============================================================================

/// 2D depth texture type
template <typename T, access A = access::sample>
struct depth2d {
    /// Read depth value at coordinate
    T read(uint2 coord) const;
    /// Read depth value at coordinate and mip level
    T read(uint2 coord, uint lod) const;
    /// Write depth value to coordinate
    void write(T depth, uint2 coord);
    /// Sample depth at normalized coordinate
    T sample(sampler s, float2 coord) const;
    /// Sample depth with LOD
    T sample(sampler s, float2 coord, level lod) const;
    /// Sample depth with comparison value
    T sample_compare(sampler s, float2 coord, float compare_value) const;
    /// Gather four depth values for bilinear filtering
    vec4<T> gather(sampler s, float2 coord, int2 offset = int2(0)) const;
    /// Gather four depth values with comparison
    vec4<T> gather_compare(sampler s, float2 coord, float compare_value, int2 offset = int2(0)) const;
    /// Return the texture width
    uint get_width(uint lod = 0) const;
    /// Return the texture height
    uint get_height(uint lod = 0) const;
    /// Return the number of mip levels
    uint get_num_mip_levels() const;
};

// ============================================================================
// depth2d_array
// ============================================================================

/// 2D depth array texture type
template <typename T, access A = access::sample>
struct depth2d_array {
    /// Read depth value at coordinate and array index
    T read(uint2 coord, uint array) const;
    /// Read depth value at coordinate, array index, and mip level
    T read(uint2 coord, uint array, uint lod) const;
    /// Sample depth at normalized coordinate and array index
    T sample(sampler s, float2 coord, uint array) const;
    /// Sample depth with LOD
    T sample(sampler s, float2 coord, uint array, level lod) const;
    /// Sample depth with comparison value
    T sample_compare(sampler s, float2 coord, uint array, float compare_value) const;
    /// Gather four depth values
    vec4<T> gather(sampler s, float2 coord, uint array, int2 offset = int2(0)) const;
    /// Gather with comparison
    vec4<T> gather_compare(sampler s, float2 coord, uint array, float compare_value, int2 offset = int2(0)) const;
    /// Return the texture width
    uint get_width(uint lod = 0) const;
    /// Return the texture height
    uint get_height(uint lod = 0) const;
    /// Return the number of array elements
    uint get_array_size() const;
    /// Return the number of mip levels
    uint get_num_mip_levels() const;
};

// ============================================================================
// depthcube
// ============================================================================

/// Cube depth texture type
template <typename T, access A = access::sample>
struct depthcube {
    /// Sample depth at direction vector
    T sample(sampler s, float3 coord) const;
    /// Sample depth with LOD
    T sample(sampler s, float3 coord, level lod) const;
    /// Sample depth with comparison value
    T sample_compare(sampler s, float3 coord, float compare_value) const;
    /// Gather four depth values
    vec4<T> gather(sampler s, float3 coord) const;
    /// Gather with comparison
    vec4<T> gather_compare(sampler s, float3 coord, float compare_value) const;
    /// Return the texture width
    uint get_width(uint lod = 0) const;
    /// Return the texture height
    uint get_height(uint lod = 0) const;
    /// Return the number of mip levels
    uint get_num_mip_levels() const;
};

// ============================================================================
// depth2d_ms (multisample)
// ============================================================================

/// 2D multisample depth texture type
template <typename T, access A = access::read>
struct depth2d_ms {
    /// Read depth sample at coordinate
    T read(uint2 coord, uint sample) const;
    /// Return the texture width
    uint get_width() const;
    /// Return the texture height
    uint get_height() const;
    /// Return the number of samples per pixel
    uint get_num_samples() const;
};

// ============================================================================
// texture_buffer
// ============================================================================

/// Texture buffer type for linear access to buffer data as formatted texels
template <typename T, access A = access::read>
struct texture_buffer {
    /// Read a texel at index
    vec4<T> read(uint index) const;
    /// Write a color to index
    void write(vec4<T> color, uint index);
    /// Return the number of texels
    uint get_width() const;
};

} // namespace metal
