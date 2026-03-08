#pragma once

namespace metal {

// ============================================================================
// Memory flags for barriers
// ============================================================================

/// Flags specifying which memory to synchronize
enum class mem_flags : unsigned {
    /// No memory fence
    mem_none = 0,
    /// Device memory fence
    mem_device = 1,
    /// Threadgroup memory fence
    mem_threadgroup = 2,
    /// Texture memory fence
    mem_texture = 4,
    /// Threadgroup and device memory fence
    mem_device_and_threadgroup = 3
};

// ============================================================================
// Barrier functions
// ============================================================================

/// Synchronize threadgroup execution and memory
/// All threads in the threadgroup must execute this before any can proceed
void threadgroup_barrier(mem_flags flags);

/// Memory fence without execution barrier
void threadgroup_mem_fence(mem_flags flags);

/// Device memory fence
void device_mem_fence(mem_flags flags);

} // namespace metal
