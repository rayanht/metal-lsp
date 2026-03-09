#pragma once
#include <metal_types.h>

namespace metal {

// ============================================================================
// Imageblock types — tile-based deferred rendering on Apple GPUs
// ============================================================================

/// Imageblock data handle for reading and writing tile memory
template <typename T>
struct imageblock {
    /// Read data from the imageblock at the given coordinates
    T read(ushort2 coord) const;

    /// Write data to the imageblock at the given coordinates
    void write(T data, ushort2 coord);

    /// Get the width of the imageblock in pixels
    ushort get_width() const;

    /// Get the height of the imageblock in pixels
    ushort get_height() const;
};

/// Imageblock slice for accessing a single field across all pixels
template <typename T>
struct imageblock_slice {
    /// Read a value from the slice at the given coordinates
    T read(ushort2 coord) const;

    /// Write a value to the slice at the given coordinates
    void write(T data, ushort2 coord);
};

} // namespace metal
