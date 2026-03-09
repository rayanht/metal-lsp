#pragma once

// Metal LSP — Compatibility header
// Force-included via -include so clangd can parse .metal files as C++.
// Maps Metal keywords to C++ constructs or erases them.
//
// The .clangd config should also include -Wno-unknown-attributes to suppress
// warnings about [[buffer(0)]], [[texture(0)]], [[position]], etc.

// ============================================================================
// Address space qualifiers — map to distinct C++ type qualifiers
//
// Metal's address spaces (device, constant, thread, threadgroup) create
// distinct pointer types for overloading. In C++ we approximate this with:
//   device    → volatile    — creates distinct type; matches atomic stubs naturally
//   constant  → const       — semantically correct (read-only buffers)
//   thread    → (nothing)   — default/local, keeps auto variables clean
//   threadgroup → (nothing) — rarely overloaded against thread
//
// Key insight: `device const T*` → `const volatile T*` is distinct from
// `constant T*` → `const T*`, resolving ALL overload collisions.
// ============================================================================
#define device volatile
#define constant const
#define thread
#define threadgroup
#define threadgroup_imageblock
#define ray_data
#define object_data

// ============================================================================
// Function qualifiers — erase them
// ============================================================================
#define kernel
#define vertex
#define fragment
#define mesh
#define object

// ============================================================================
// Metal attribute parameters used in [[...]] function parameter attributes
// These appear after the type in kernel signatures, e.g.:
//   kernel void foo(uint id [[thread_position_in_grid]])
// clangd sees [[thread_position_in_grid]] as a C++ attribute and ignores it,
// but if used bare (without [[]]) we need them to resolve to nothing.
// ============================================================================
#define thread_position_in_grid
#define thread_position_in_threadgroup
#define threadgroup_position_in_grid
#define threads_per_threadgroup
#define thread_index_in_threadgroup
#define threadgroups_per_grid
#define thread_execution_width
#define simdgroup_index_in_threadgroup
#define threads_per_simdgroup
#define dispatch_threads_per_threadgroup

// Vertex/fragment stage-in and resource binding attributes
#define stage_in

// Note: buffer(n), texture(n), sampler(n), color(n) are used inside [[...]]
// C++ attribute syntax which clangd already handles. Defining them as macros
// would conflict with the sampler type and common identifiers.

// Note: position, point_size, flat, etc. are also [[...]] attributes.
// We do NOT #define these — they conflict with common struct member names
// (e.g., `float4 position` would be mangled to `float4`).

#define centroid_perspective
#define centroid_no_perspective
#define sample_perspective
#define sample_no_perspective
#define center_perspective
#define center_no_perspective

// Mesh/object shader attributes
#define object_thread_position_in_grid
#define mesh_thread_position_in_grid
#define meshgrid_position_in_grid
#define object_thread_position_in_threadgroup
#define mesh_thread_position_in_threadgroup
#define threads_per_object_threadgroup
#define threads_per_mesh_threadgroup

// ============================================================================
// Half-precision literal suffix
// Metal uses 1.0h for half literals. The operator""h definition lives in
// stubs/metal_stdlib rather than here, because clangd's preamble PCH
// serialization can lose UDL operators from force-included headers.
// ============================================================================

// ============================================================================
// as_type<T> — Metal's bit-cast function
// ============================================================================
template <typename T, typename U>
T as_type(U x) { return *reinterpret_cast<const T*>(&x); }

// ============================================================================
// select — ternary select function
// ============================================================================

// ============================================================================
// Texture access enum — needed for texture template parameters
// e.g., texture2d<float, access::read>
// ============================================================================
namespace metal {
    enum class access { read, write, read_write, sample };
}

// ============================================================================
// Pixel format enum — used with imageblocks
// ============================================================================
namespace metal {
    enum class pixel_format {
        rgba8unorm, bgra8unorm, rgba16float, rgba32float,
        r8unorm, r16float, r32float, rg8unorm, rg16float, rg32float
    };
}
