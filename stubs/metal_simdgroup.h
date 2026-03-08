#pragma once
#include <metal_types.h>

namespace metal {

// ============================================================================
// SIMD memory flags (reuse mem_flags from compute)
// ============================================================================

// Forward declaration — mem_flags is defined in metal_compute.h
enum class mem_flags : unsigned;

// ============================================================================
// SIMD group barrier
// ============================================================================

/// Synchronize execution within a SIMD group
void simdgroup_barrier(mem_flags flags);

// ============================================================================
// SIMD group reductions
// ============================================================================

/// Sum all values across the SIMD group
template <typename T> T simd_sum(T value);
/// Multiply all values across the SIMD group
template <typename T> T simd_product(T value);
/// Minimum of all values across the SIMD group
template <typename T> T simd_min(T value);
/// Maximum of all values across the SIMD group
template <typename T> T simd_max(T value);
/// Bitwise AND across the SIMD group
template <typename T> T simd_and(T value);
/// Bitwise OR across the SIMD group
template <typename T> T simd_or(T value);
/// Bitwise XOR across the SIMD group
template <typename T> T simd_xor(T value);

float simd_sum(float value);
float simd_product(float value);
float simd_min(float value);
float simd_max(float value);
half simd_sum(half value);
half simd_product(half value);
half simd_min(half value);
half simd_max(half value);
int simd_sum(int value);
int simd_min(int value);
int simd_max(int value);
uint simd_sum(uint value);
uint simd_min(uint value);
uint simd_max(uint value);
uint simd_and(uint value);
uint simd_or(uint value);
uint simd_xor(uint value);

// ============================================================================
// SIMD group prefix/scan operations
// ============================================================================

/// Exclusive prefix sum across the SIMD group
template <typename T> T simd_prefix_exclusive_sum(T value);
/// Inclusive prefix sum across the SIMD group
template <typename T> T simd_prefix_inclusive_sum(T value);
/// Exclusive prefix product across the SIMD group
template <typename T> T simd_prefix_exclusive_product(T value);
/// Inclusive prefix product across the SIMD group
template <typename T> T simd_prefix_inclusive_product(T value);

float simd_prefix_exclusive_sum(float value);
float simd_prefix_inclusive_sum(float value);
half simd_prefix_exclusive_sum(half value);
half simd_prefix_inclusive_sum(half value);
int simd_prefix_exclusive_sum(int value);
int simd_prefix_inclusive_sum(int value);
uint simd_prefix_exclusive_sum(uint value);
uint simd_prefix_inclusive_sum(uint value);

// ============================================================================
// SIMD group shuffle operations
// ============================================================================

/// Read value from lane at index
template <typename T> T simd_shuffle(T value, ushort lane);
/// Read value from lane (current_lane - delta)
template <typename T> T simd_shuffle_up(T value, ushort delta);
/// Read value from lane (current_lane + delta)
template <typename T> T simd_shuffle_down(T value, ushort delta);
/// Read value from lane (current_lane XOR mask)
template <typename T> T simd_shuffle_xor(T value, ushort mask);
/// Broadcast value from specified lane to all lanes
template <typename T> T simd_broadcast(T value, ushort lane);

float simd_shuffle(float value, ushort lane);
float simd_shuffle_up(float value, ushort delta);
float simd_shuffle_down(float value, ushort delta);
float simd_shuffle_xor(float value, ushort mask);
float simd_broadcast(float value, ushort lane);
half simd_shuffle(half value, ushort lane);
half simd_shuffle_down(half value, ushort delta);
int simd_shuffle(int value, ushort lane);
int simd_shuffle_down(int value, ushort delta);
uint simd_shuffle(uint value, ushort lane);
uint simd_shuffle_down(uint value, ushort delta);
uint simd_broadcast(uint value, ushort lane);

// ============================================================================
// SIMD group ballot
// ============================================================================

/// Return bitmask of lanes where predicate is true
unsigned long long simd_ballot(bool predicate);

/// Return true if predicate is true for all active lanes
bool simd_all(bool predicate);
/// Return true if predicate is true for any active lane
bool simd_any(bool predicate);

// ============================================================================
// Quad group operations (2x2 quad within SIMD group)
// ============================================================================

/// Sum values across the quad group
template <typename T> T quad_sum(T value);
/// Shuffle within quad group
template <typename T> T quad_shuffle(T value, ushort lane);
/// Shuffle XOR within quad group
template <typename T> T quad_shuffle_xor(T value, ushort mask);
/// Broadcast within quad group
template <typename T> T quad_broadcast(T value, ushort lane);

float quad_sum(float value);
half quad_sum(half value);

// ============================================================================
// SIMD-group matrix operations (Apple GPU family 7+)
// ============================================================================

/// SIMD-group matrix for cooperative matrix multiply
template <typename T, int Rows, int Cols>
struct simdgroup_matrix {
    simdgroup_matrix() = default;
    /// Construct with diagonal value
    simdgroup_matrix(T value);
};

/// Cooperative matrix multiply-accumulate: D = A * B + C
template <typename T, typename U, int M, int N, int K>
void simdgroup_multiply_accumulate(
    simdgroup_matrix<T, M, N>& D,
    simdgroup_matrix<U, M, K> A,
    simdgroup_matrix<U, K, N> B,
    simdgroup_matrix<T, M, N> C
);

/// Load simdgroup_matrix from device memory
template <typename T, int R, int C>
void simdgroup_load(simdgroup_matrix<T, R, C>& m, const T* src, ulong elements_per_row = 0, ulong2 matrix_origin = ulong2(0), bool transpose = false);

/// Store simdgroup_matrix to device memory
template <typename T, int R, int C>
void simdgroup_store(simdgroup_matrix<T, R, C> m, T* dst, ulong elements_per_row = 0, ulong2 matrix_origin = ulong2(0), bool transpose = false);

/// Multiply two simdgroup matrices
template <typename T, int M, int N, int K>
void simdgroup_multiply(simdgroup_matrix<T, M, N>& D, simdgroup_matrix<T, M, K> A, simdgroup_matrix<T, K, N> B);

/// Common simdgroup_matrix type aliases
typedef simdgroup_matrix<float, 8, 8> simdgroup_float8x8;
typedef simdgroup_matrix<half, 8, 8> simdgroup_half8x8;

} // namespace metal
