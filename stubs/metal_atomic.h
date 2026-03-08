#pragma once
#include <metal_types.h>

namespace metal {

// ============================================================================
// Memory order enum
// ============================================================================

/// Memory ordering for atomic operations
enum memory_order {
    memory_order_relaxed,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel
};

// ============================================================================
// Atomic type
// ============================================================================

/// Atomic type for lock-free atomic operations
template <typename T>
struct atomic {
    /// Atomically store a value
    void store(T val, memory_order order = memory_order_relaxed) volatile;
    /// Atomically load a value
    T load(memory_order order = memory_order_relaxed) const volatile;
    /// Atomically exchange and return old value
    T exchange(T val, memory_order order = memory_order_relaxed) volatile;
    /// Atomically compare and exchange
    bool compare_exchange_weak(T& expected, T desired, memory_order succ = memory_order_relaxed, memory_order fail = memory_order_relaxed) volatile;
    /// Atomically add and return old value
    T fetch_add(T val, memory_order order = memory_order_relaxed) volatile;
    /// Atomically subtract and return old value
    T fetch_sub(T val, memory_order order = memory_order_relaxed) volatile;
    /// Atomically AND and return old value
    T fetch_and(T val, memory_order order = memory_order_relaxed) volatile;
    /// Atomically OR and return old value
    T fetch_or(T val, memory_order order = memory_order_relaxed) volatile;
    /// Atomically XOR and return old value
    T fetch_xor(T val, memory_order order = memory_order_relaxed) volatile;
    /// Atomically compute minimum and return old value
    T fetch_min(T val, memory_order order = memory_order_relaxed) volatile;
    /// Atomically compute maximum and return old value
    T fetch_max(T val, memory_order order = memory_order_relaxed) volatile;
};

/// Atomic unsigned integer type
typedef atomic<uint> atomic_uint;
/// Atomic signed integer type
typedef atomic<int> atomic_int;
/// Atomic boolean type
typedef atomic<bool> atomic_bool;
/// Atomic float type (Metal 3.0+)
typedef atomic<float> atomic_float;

// ============================================================================
// Free-function atomic operations (C-style API)
// ============================================================================

/// Atomically add val to object, return old value
template <typename T>
T atomic_fetch_add_explicit(volatile atomic<T>* object, T val, memory_order order);

/// Atomically subtract val from object, return old value
template <typename T>
T atomic_fetch_sub_explicit(volatile atomic<T>* object, T val, memory_order order);

/// Atomically compute min of object and val, return old value
template <typename T>
T atomic_fetch_min_explicit(volatile atomic<T>* object, T val, memory_order order);

/// Atomically compute max of object and val, return old value
template <typename T>
T atomic_fetch_max_explicit(volatile atomic<T>* object, T val, memory_order order);

/// Atomically AND val with object, return old value
template <typename T>
T atomic_fetch_and_explicit(volatile atomic<T>* object, T val, memory_order order);

/// Atomically OR val with object, return old value
template <typename T>
T atomic_fetch_or_explicit(volatile atomic<T>* object, T val, memory_order order);

/// Atomically XOR val with object, return old value
template <typename T>
T atomic_fetch_xor_explicit(volatile atomic<T>* object, T val, memory_order order);

/// Atomically exchange and return old value
template <typename T>
T atomic_exchange_explicit(volatile atomic<T>* object, T desired, memory_order order);

/// Atomically compare and exchange; returns true on success
template <typename T>
bool atomic_compare_exchange_weak_explicit(volatile atomic<T>* object, T* expected, T desired, memory_order succ, memory_order fail);

/// Atomically store a value
template <typename T>
void atomic_store_explicit(volatile atomic<T>* object, T desired, memory_order order);

/// Atomically load a value
template <typename T>
T atomic_load_explicit(volatile atomic<T>* object, memory_order order);

// Concrete overloads to avoid template deduction issues with int literals
uint atomic_fetch_add_explicit(volatile atomic<uint>* object, int val, memory_order order);
uint atomic_fetch_sub_explicit(volatile atomic<uint>* object, int val, memory_order order);
void atomic_store_explicit(volatile atomic<uint>* object, int desired, memory_order order);
void atomic_store_explicit(volatile atomic<int>* object, uint desired, memory_order order);
int atomic_fetch_add_explicit(volatile atomic<int>* object, uint val, memory_order order);

} // namespace metal
