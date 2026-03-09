#pragma once
#include <metal_types.h>

// Save and undefine the 'mesh' macro from the compat header
// so we can declare the mesh output type
#pragma push_macro("mesh")
#undef mesh

namespace metal {

// ============================================================================
// Mesh shader types — Metal 3.0+
// ============================================================================

/// Properties for configuring the mesh grid
struct mesh_grid_properties {
    /// Set the number of threadgroups for the mesh shader stage
    void set_threadgroups_per_grid(uint3 threadgroups);
};

/// Mesh output topology types
enum class topology {
    point,
    line,
    triangle
};

/// Mesh output object for emitting primitives and vertices from mesh shaders
template <typename VertexType, typename PrimitiveType, unsigned MaxVertices, unsigned MaxPrimitives, topology Topology = topology::triangle>
struct mesh {
    /// Set the number of vertices this mesh shader will output
    void set_primitive_count(unsigned count);

    /// Access a vertex by index for writing
    VertexType& set_vertex(unsigned index);

    /// Access a primitive by index for writing
    PrimitiveType& set_primitive(unsigned index);

    /// Set the indices for a triangle primitive
    void set_index(unsigned primitive_index, uint3 indices);
    /// Set the indices for a line primitive (2 indices)
    void set_index(unsigned primitive_index, uint2 indices);
    /// Set the index for a point primitive (1 index)
    void set_index(unsigned primitive_index, unsigned index);
};

} // namespace metal

// Restore the 'mesh' macro
#pragma pop_macro("mesh")
