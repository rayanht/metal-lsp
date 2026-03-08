#pragma once
#include <metal_types.h>

namespace metal {

// Helper: map int N to vecN<T>
template <typename T, int N> struct _vec_of;
template <typename T> struct _vec_of<T, 2> { typedef vec2<T> type; };
template <typename T> struct _vec_of<T, 3> { typedef vec3<T> type; };
template <typename T> struct _vec_of<T, 4> { typedef vec4<T> type; };

// ============================================================================
// Matrix types — column-major storage
// ============================================================================

/// Column-major matrix with C columns and R rows
template <typename T, int C, int R>
struct matrix {
    typedef typename _vec_of<T, R>::type column_type;
    typedef typename _vec_of<T, C>::type row_type;

    column_type columns[C];

    /// Default constructor
    matrix() = default;

    /// Construct from scalar (fills diagonal)
    explicit matrix(T v);

    /// Construct from column vectors (2 columns)
    matrix(column_type c0, column_type c1);
    /// Construct from column vectors (3 columns)
    matrix(column_type c0, column_type c1, column_type c2);
    /// Construct from column vectors (4 columns)
    matrix(column_type c0, column_type c1, column_type c2, column_type c3);

    /// Construct from individual elements (row-major order, 4 elements for 2x2)
    matrix(T m00, T m01, T m02, T m03);
    /// Construct from individual elements (9 elements for 3x3)
    matrix(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22);
    /// Construct from individual elements (16 elements for 4x4)
    matrix(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13,
           T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33);
    /// Construct from individual elements (6 elements for 2x3 / 3x2)
    matrix(T m00, T m01, T m02, T m03, T m04, T m05);
    /// Construct from individual elements (8 elements for 2x4 / 4x2)
    matrix(T m00, T m01, T m02, T m03, T m04, T m05, T m06, T m07);
    /// Construct from individual elements (12 elements for 3x4 / 4x3)
    matrix(T m00, T m01, T m02, T m03, T m04, T m05, T m06, T m07,
           T m08, T m09, T m10, T m11);

    /// Access column by index
    column_type operator[](int i) const { return columns[i]; }
    /// Access column by index (mutable)
    column_type& operator[](int i) { return columns[i]; }

    /// Matrix-matrix addition
    matrix operator+(matrix b) const;
    /// Matrix-matrix subtraction
    matrix operator-(matrix b) const;
    /// Matrix-scalar multiplication
    matrix operator*(T b) const;
    /// Negation
    matrix operator-() const;
};

/// Matrix-matrix multiply (CxR * C2xC -> C2xR)
template <typename T, int C, int R, int C2>
matrix<T, C2, R> operator*(matrix<T, C, R> a, matrix<T, C2, C> b);

/// Scalar-matrix multiply
template <typename T, int C, int R>
matrix<T, C, R> operator*(T a, matrix<T, C, R> b);

/// Scalar-matrix multiply (different scalar type, e.g., double * float matrix)
template <typename S, typename T, int C, int R>
matrix<T, C, R> operator*(S a, matrix<T, C, R> b);

/// Matrix-scalar multiply (different scalar type)
template <typename T, int C, int R, typename S>
matrix<T, C, R> operator*(matrix<T, C, R> a, S b);

/// Transpose a matrix
template <typename T, int C, int R>
matrix<T, R, C> transpose(matrix<T, C, R> m);

/// Determinant of a square matrix
template <typename T, int N>
T determinant(matrix<T, N, N> m);

// ============================================================================
// Concrete matrix * vector overloads for clangd resolution
// ============================================================================

/// Multiply 4x4 matrix by float4 vector
float4 operator*(matrix<float, 4, 4> m, float4 v);
/// Multiply 3x3 matrix by float3 vector
float3 operator*(matrix<float, 3, 3> m, float3 v);
/// Multiply 2x2 matrix by float2 vector
float2 operator*(matrix<float, 2, 2> m, float2 v);
/// Multiply 4x4 matrix by half4 vector
half4 operator*(matrix<half, 4, 4> m, half4 v);
/// Multiply 3x3 matrix by half3 vector
half3 operator*(matrix<half, 3, 3> m, half3 v);
/// Multiply 2x2 matrix by half2 vector
half2 operator*(matrix<half, 2, 2> m, half2 v);

/// Multiply float4 row vector by 4x4 matrix
float4 operator*(float4 v, matrix<float, 4, 4> m);
/// Multiply float3 row vector by 3x3 matrix
float3 operator*(float3 v, matrix<float, 3, 3> m);
/// Multiply float2 row vector by 2x2 matrix
float2 operator*(float2 v, matrix<float, 2, 2> m);

/// Multiply 3x4 matrix by float3 vector
float4 operator*(matrix<float, 3, 4> m, float3 v);
/// Multiply 4x3 matrix by float4 vector
float3 operator*(matrix<float, 4, 3> m, float4 v);

// ============================================================================
// float matrix aliases
// ============================================================================

typedef matrix<float, 2, 2> float2x2;
typedef matrix<float, 2, 3> float2x3;
typedef matrix<float, 2, 4> float2x4;
typedef matrix<float, 3, 2> float3x2;
typedef matrix<float, 3, 3> float3x3;
typedef matrix<float, 3, 4> float3x4;
typedef matrix<float, 4, 2> float4x2;
typedef matrix<float, 4, 3> float4x3;
typedef matrix<float, 4, 4> float4x4;

// ============================================================================
// half matrix aliases
// ============================================================================

typedef matrix<half, 2, 2> half2x2;
typedef matrix<half, 2, 3> half2x3;
typedef matrix<half, 2, 4> half2x4;
typedef matrix<half, 3, 2> half3x2;
typedef matrix<half, 3, 3> half3x3;
typedef matrix<half, 3, 4> half3x4;
typedef matrix<half, 4, 2> half4x2;
typedef matrix<half, 4, 3> half4x3;
typedef matrix<half, 4, 4> half4x4;

} // namespace metal
