#pragma once
#include <metal_types.h>

namespace metal {
namespace raytracing {

// ============================================================================
// Ray type
// ============================================================================

/// A ray with an origin, direction, and min/max distance
struct ray {
    /// The ray origin
    float3 origin;
    /// The ray direction
    float3 direction;
    /// Minimum ray distance
    float min_distance;
    /// Maximum ray distance
    float max_distance;

    /// Construct a ray
    ray() = default;
    ray(float3 origin, float3 direction, float min_distance = 0.0f, float max_distance = __builtin_inff());
};

// ============================================================================
// Intersection result types
// ============================================================================

/// Intersection type enum
enum class intersection_type { none, triangle, bounding_box };

/// Triangle intersection result
struct intersection_result {
    /// The type of geometry hit
    intersection_type type;
    /// Distance along the ray to the intersection
    float distance;
    /// Barycentric coordinates of the hit point (triangle only)
    float2 triangle_barycentric_coord;
    /// Index of the intersected primitive
    unsigned int primitive_id;
    /// Index of the intersected geometry
    unsigned int geometry_id;
    /// Index of the intersected instance (for instanced acceleration structures)
    unsigned int instance_id;

    /// Whether the intersection hit anything
    bool accept() const;
};

// ============================================================================
// Acceleration structures
// ============================================================================

/// Acceleration structure for ray-scene intersection queries
struct primitive_acceleration_structure {
};

/// Instanced acceleration structure containing references to primitive acceleration structures
struct instance_acceleration_structure {
};

/// Aliases for common usage
using acceleration_structure = instance_acceleration_structure;

// ============================================================================
// Intersector
// ============================================================================

/// Tag types for intersector configuration
struct triangle_data {};
struct instancing {};

/// Performs ray-acceleration structure intersection queries
template <typename... Tags>
struct intersector {
    /// The result type returned by intersect()
    using result_type = intersection_result;

    intersector() = default;

    /// Set whether to accept any hit or find the closest hit
    void accept_any_intersection(bool accept);

    /// Set whether to force opaque behavior on all geometry
    void force_opacity(bool opaque);

    /// Set whether to assume instancing
    void assume_geometry_type(bool triangles);

    /// Intersect a ray with an acceleration structure
    result_type intersect(ray r, primitive_acceleration_structure accel_struct,
                          unsigned int intersection_function_table_offset = 0);
    result_type intersect(ray r, instance_acceleration_structure accel_struct,
                          unsigned int intersection_function_table_offset = 0);
};

// ============================================================================
// Intersection function table
// ============================================================================

/// Table of custom intersection functions for procedural geometry
struct intersection_function_table {
};

} // namespace raytracing
} // namespace metal
