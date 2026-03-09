#pragma once
#include <metal_types.h>

namespace metal {

// ============================================================================
// Visible function tables — indirect function calls (Metal 3.0+)
// ============================================================================

// Helper to extract return type and args from function type
template <typename Fn> struct _vft_caller;

template <typename Ret, typename... Args>
struct _vft_caller<Ret(Args...)> {
    struct proxy {
        unsigned _index;
        Ret operator()(Args... args) const;
    };
};

/// A table of function pointers for dynamic dispatch in Metal shaders
template <typename Fn>
struct visible_function_table {
    /// Access a function at the given index, returning a callable proxy
    typename _vft_caller<Fn>::proxy operator[](unsigned index) const {
        return {index};
    }
};

} // namespace metal
