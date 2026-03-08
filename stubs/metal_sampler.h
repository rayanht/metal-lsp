#pragma once

namespace metal {

// ============================================================================
// Sampler enums
// ============================================================================

/// Texture coordinate mode
enum class coord {
    /// Normalized coordinates [0.0, 1.0]
    normalized,
    /// Pixel coordinates [0, width/height]
    pixel
};

/// Texture addressing mode
enum class address {
    /// Clamp to edge texel color
    clamp_to_edge,
    /// Clamp to border color
    clamp_to_border,
    /// Clamp to zero
    clamp_to_zero,
    /// Repeat (wrap around)
    repeat,
    /// Mirrored repeat
    mirrored_repeat
};

/// Texture sampling filter
enum class filter {
    /// Nearest-neighbor sampling
    nearest,
    /// Linear (bilinear) sampling
    linear
};

/// Mipmap filter
enum class mip_filter {
    /// No mipmapping
    none,
    /// Nearest mip level
    nearest,
    /// Linear interpolation between mip levels
    linear
};

/// Depth compare function for shadow sampling
enum class compare_func {
    never,
    less,
    less_equal,
    greater,
    greater_equal,
    equal,
    not_equal,
    always
};

/// Border color for clamp_to_border addressing
enum class border_color {
    transparent_black,
    opaque_black,
    opaque_white
};

// ============================================================================
// Sampler type
// ============================================================================

/// Sampler state for texture sampling operations
struct sampler {
    /// Default constructor
    sampler() = default;
};

// Constexpr sampler — in Metal you declare these with attributes, but for
// clangd we just need the type to exist.
struct constexpr_sampler : sampler {
    constexpr_sampler() = default;
};

} // namespace metal
