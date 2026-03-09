#pragma once

namespace metal {

// ============================================================================
// Scalar type aliases
// ============================================================================

/// 16-bit floating-point type
typedef __fp16 half;

/// Unsigned 8-bit integer
typedef unsigned char uchar;

/// Unsigned 16-bit integer
typedef unsigned short ushort;

/// Unsigned 32-bit integer
typedef unsigned int uint;

/// Unsigned 64-bit integer
typedef unsigned long ulong;

/// Signed 64-bit integer
typedef long long ptrdiff_t;

/// Unsigned pointer-sized integer
typedef unsigned long size_t;

/// Signed 8-bit integer
typedef signed char int8_t;
/// Unsigned 8-bit integer
typedef unsigned char uint8_t;
/// Signed 16-bit integer
typedef short int16_t;
/// Unsigned 16-bit integer
typedef unsigned short uint16_t;
/// Signed 32-bit integer
typedef int int32_t;
/// Unsigned 32-bit integer
typedef unsigned int uint32_t;
/// Signed 64-bit integer
typedef long long int64_t;
/// Unsigned 64-bit integer
typedef unsigned long long uint64_t;

// ============================================================================
// Forward declarations
// ============================================================================

template <typename T> struct vec2;
template <typename T> struct vec3;
template <typename T> struct vec4;

// ============================================================================
// SFINAE helper — constrains free scalar-vector operators
// ============================================================================

template <bool, typename T = void> struct _enable_if;
template <typename T> struct _enable_if<true, T> { typedef T type; };

// ============================================================================
// Swizzle proxy — provides .xy, .xyz etc. as read-only accessors
// ============================================================================

// 2-from-3 swizzle proxy (returns vec2 from vec3/vec4)
template <typename T, int A, int B>
struct _swiz2 {
    T _data[4]; // max possible source size
    operator vec2<T>() const;
};

// 3-from-4 swizzle proxy (returns vec3 from vec4)
template <typename T, int A, int B, int C>
struct _swiz3 {
    T _data[4];
    operator vec3<T>() const;
};

// ============================================================================
// vec2<T>
// ============================================================================

template <typename T>
struct vec2 {
    union {
        struct { T x, y; };
        struct { T r, g; };
        struct { T s, t; };
    };

    /// Default constructor
    vec2() = default;
    /// Construct from scalar (broadcast)
    vec2(T v) : x(v), y(v) {}
    /// Construct from components
    vec2(T x, T y) : x(x), y(y) {}
    /// Convert from another element type
    template <typename U>
    explicit vec2(vec2<U> v) : x(T(v.x)), y(T(v.y)) {}
    /// Copy from volatile (needed for device→volatile mapping)
    vec2(const volatile vec2& v) : x(v.x), y(v.y) {}

    T operator[](int i) const { return (&x)[i]; }
    T& operator[](int i) { return (&x)[i]; }

    // Volatile-qualified assignment (needed for device→volatile mapping)
    volatile vec2& operator=(const vec2& v) volatile { x=v.x; y=v.y; return *this; }

    // Arithmetic operators (const and const volatile)
    vec2 operator+(vec2 b) const { return {T(x+b.x), T(y+b.y)}; }
    vec2 operator-(vec2 b) const { return {T(x-b.x), T(y-b.y)}; }
    vec2 operator*(vec2 b) const { return {T(x*b.x), T(y*b.y)}; }
    vec2 operator/(vec2 b) const { return {T(x/b.x), T(y/b.y)}; }
    vec2 operator+(T b) const { return {T(x+b), T(y+b)}; }
    vec2 operator-(T b) const { return {T(x-b), T(y-b)}; }
    vec2 operator*(T b) const { return {T(x*b), T(y*b)}; }
    vec2 operator/(T b) const { return {T(x/b), T(y/b)}; }
    // Volatile-qualified arithmetic (for device pointer access)
    vec2 operator+(vec2 b) const volatile { return {T(x+b.x), T(y+b.y)}; }
    vec2 operator-(vec2 b) const volatile { return {T(x-b.x), T(y-b.y)}; }
    vec2 operator*(vec2 b) const volatile { return {T(x*b.x), T(y*b.y)}; }
    vec2 operator/(vec2 b) const volatile { return {T(x/b.x), T(y/b.y)}; }
    vec2 operator+(T b) const volatile { return {T(x+b), T(y+b)}; }
    vec2 operator-(T b) const volatile { return {T(x-b), T(y-b)}; }
    vec2 operator*(T b) const volatile { return {T(x*b), T(y*b)}; }
    vec2 operator/(T b) const volatile { return {T(x/b), T(y/b)}; }
    vec2& operator+=(vec2 b) { x+=b.x; y+=b.y; return *this; }
    vec2& operator-=(vec2 b) { x-=b.x; y-=b.y; return *this; }
    vec2& operator*=(vec2 b) { x*=b.x; y*=b.y; return *this; }
    vec2& operator/=(vec2 b) { x/=b.x; y/=b.y; return *this; }
    vec2& operator+=(T b) { x+=b; y+=b; return *this; }
    vec2& operator-=(T b) { x-=b; y-=b; return *this; }
    vec2& operator*=(T b) { x*=b; y*=b; return *this; }
    vec2& operator/=(T b) { x/=b; y/=b; return *this; }
    vec2 operator-() const { return {T(-x), T(-y)}; }
    bool operator==(vec2 b) const { return x==b.x && y==b.y; }
    bool operator!=(vec2 b) const { return x!=b.x || y!=b.y; }
    bool operator<(vec2 b) const { return x<b.x && y<b.y; }
    bool operator>(vec2 b) const { return x>b.x && y>b.y; }
    bool operator<=(vec2 b) const { return x<=b.x && y<=b.y; }
    bool operator>=(vec2 b) const { return x>=b.x && y>=b.y; }

    // Bitwise operators
    vec2 operator%(vec2 b) const { return {T(x%b.x), T(y%b.y)}; }
    vec2 operator&(vec2 b) const { return {T(x&b.x), T(y&b.y)}; }
    vec2 operator|(vec2 b) const { return {T(x|b.x), T(y|b.y)}; }
    vec2 operator^(vec2 b) const { return {T(x^b.x), T(y^b.y)}; }
    vec2 operator~() const { return {T(~x), T(~y)}; }
    vec2 operator<<(vec2 b) const { return {T(x<<b.x), T(y<<b.y)}; }
    vec2 operator>>(vec2 b) const { return {T(x>>b.x), T(y>>b.y)}; }
    vec2 operator<<(int b) const { return {T(x<<b), T(y<<b)}; }
    vec2 operator>>(int b) const { return {T(x>>b), T(y>>b)}; }
    vec2& operator%=(vec2 b) { x%=b.x; y%=b.y; return *this; }
    vec2& operator&=(vec2 b) { x&=b.x; y&=b.y; return *this; }
    vec2& operator|=(vec2 b) { x|=b.x; y|=b.y; return *this; }
    vec2& operator^=(vec2 b) { x^=b.x; y^=b.y; return *this; }
    vec2& operator<<=(vec2 b) { x<<=b.x; y<<=b.y; return *this; }
    vec2& operator>>=(vec2 b) { x>>=b.x; y>>=b.y; return *this; }
    vec2& operator<<=(int b) { x<<=b; y<<=b; return *this; }
    vec2& operator>>=(int b) { x>>=b; y>>=b; return *this; }

    // Logical
    bool operator!() const { return !x && !y; }
};

template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec2<T> operator*(S a, vec2<T> b) { return {T(a*b.x), T(a*b.y)}; }
template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec2<T> operator+(S a, vec2<T> b) { return {T(a+b.x), T(a+b.y)}; }
template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec2<T> operator-(S a, vec2<T> b) { return {T(a-b.x), T(a-b.y)}; }
template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec2<T> operator/(S a, vec2<T> b) { return {T(a/b.x), T(a/b.y)}; }

// ============================================================================
// vec3<T>
// ============================================================================

template <typename T>
struct vec3 {
    union {
        struct { T x, y, z; };
        struct { T r, g, b; };
    };
    // Swizzle properties — these overlay the same memory
    vec2<T>& xy;  // Note: reference to self — only for clangd autocomplete, not runtime
    vec2<T>& rg;

    vec3() : x{}, y{}, z{}, xy(*(vec2<T>*)this), rg(*(vec2<T>*)this) {}
    /// Construct from scalar (broadcast)
    vec3(T v) : x(v), y(v), z(v), xy(*(vec2<T>*)this), rg(*(vec2<T>*)this) {}
    /// Construct from components
    vec3(T x, T y, T z) : x(x), y(y), z(z), xy(*(vec2<T>*)this), rg(*(vec2<T>*)this) {}
    /// Construct from vec2 + scalar
    vec3(vec2<T> v, T z) : x(v.x), y(v.y), z(z), xy(*(vec2<T>*)this), rg(*(vec2<T>*)this) {}
    /// Construct from scalar + vec2
    vec3(T x, vec2<T> v) : x(x), y(v.x), z(v.y), xy(*(vec2<T>*)this), rg(*(vec2<T>*)this) {}
    /// Convert from another element type
    template <typename U>
    explicit vec3(vec3<U> v) : x(T(v.x)), y(T(v.y)), z(T(v.z)), xy(*(vec2<T>*)this), rg(*(vec2<T>*)this) {}
    /// Copy
    vec3(const vec3& v) : x(v.x), y(v.y), z(v.z), xy(*(vec2<T>*)this), rg(*(vec2<T>*)this) {}
    /// Copy from volatile (needed for device→volatile mapping)
    vec3(const volatile vec3& v) : x(v.x), y(v.y), z(v.z), xy(*(vec2<T>*)this), rg(*(vec2<T>*)this) {}
    vec3& operator=(const vec3& v) { x=v.x; y=v.y; z=v.z; return *this; }
    // Volatile-qualified assignment (needed for device→volatile mapping)
    volatile vec3& operator=(const vec3& v) volatile { x=v.x; y=v.y; z=v.z; return *this; }

    T operator[](int i) const { return (&x)[i]; }
    T& operator[](int i) { return (&x)[i]; }

    vec3 operator+(vec3 b) const { return {T(x+b.x), T(y+b.y), T(z+b.z)}; }
    vec3 operator-(vec3 b) const { return {T(x-b.x), T(y-b.y), T(z-b.z)}; }
    vec3 operator*(vec3 b) const { return {T(x*b.x), T(y*b.y), T(z*b.z)}; }
    vec3 operator/(vec3 b) const { return {T(x/b.x), T(y/b.y), T(z/b.z)}; }
    vec3 operator+(T b) const { return {T(x+b), T(y+b), T(z+b)}; }
    vec3 operator-(T b) const { return {T(x-b), T(y-b), T(z-b)}; }
    vec3 operator*(T b) const { return {T(x*b), T(y*b), T(z*b)}; }
    vec3 operator/(T b) const { return {T(x/b), T(y/b), T(z/b)}; }
    // Volatile-qualified arithmetic (for device pointer access)
    vec3 operator+(vec3 b) const volatile { return {T(x+b.x), T(y+b.y), T(z+b.z)}; }
    vec3 operator-(vec3 b) const volatile { return {T(x-b.x), T(y-b.y), T(z-b.z)}; }
    vec3 operator*(vec3 b) const volatile { return {T(x*b.x), T(y*b.y), T(z*b.z)}; }
    vec3 operator/(vec3 b) const volatile { return {T(x/b.x), T(y/b.y), T(z/b.z)}; }
    vec3 operator+(T b) const volatile { return {T(x+b), T(y+b), T(z+b)}; }
    vec3 operator-(T b) const volatile { return {T(x-b), T(y-b), T(z-b)}; }
    vec3 operator*(T b) const volatile { return {T(x*b), T(y*b), T(z*b)}; }
    vec3 operator/(T b) const volatile { return {T(x/b), T(y/b), T(z/b)}; }
    vec3 operator-() const volatile { return {T(-x), T(-y), T(-z)}; }
    vec3& operator+=(vec3 b) { x+=b.x; y+=b.y; z+=b.z; return *this; }
    vec3& operator-=(vec3 b) { x-=b.x; y-=b.y; z-=b.z; return *this; }
    vec3& operator*=(vec3 b) { x*=b.x; y*=b.y; z*=b.z; return *this; }
    vec3& operator/=(vec3 b) { x/=b.x; y/=b.y; z/=b.z; return *this; }
    vec3& operator+=(T b) { x+=b; y+=b; z+=b; return *this; }
    vec3& operator-=(T b) { x-=b; y-=b; z-=b; return *this; }
    vec3& operator*=(T b) { x*=b; y*=b; z*=b; return *this; }
    vec3& operator/=(T b) { x/=b; y/=b; z/=b; return *this; }
    vec3 operator-() const { return {T(-x), T(-y), T(-z)}; }
    bool operator==(vec3 b) const { return x==b.x && y==b.y && z==b.z; }
    bool operator!=(vec3 b) const { return !(*this == b); }
    bool operator<(vec3 b) const { return x<b.x && y<b.y && z<b.z; }
    bool operator>(vec3 b) const { return x>b.x && y>b.y && z>b.z; }
    bool operator<=(vec3 b) const { return x<=b.x && y<=b.y && z<=b.z; }
    bool operator>=(vec3 b) const { return x>=b.x && y>=b.y && z>=b.z; }

    vec3 operator%(vec3 b) const { return {T(x%b.x), T(y%b.y), T(z%b.z)}; }
    vec3 operator&(vec3 b) const { return {T(x&b.x), T(y&b.y), T(z&b.z)}; }
    vec3 operator|(vec3 b) const { return {T(x|b.x), T(y|b.y), T(z|b.z)}; }
    vec3 operator^(vec3 b) const { return {T(x^b.x), T(y^b.y), T(z^b.z)}; }
    vec3 operator~() const { return {T(~x), T(~y), T(~z)}; }
    vec3 operator<<(vec3 b) const { return {T(x<<b.x), T(y<<b.y), T(z<<b.z)}; }
    vec3 operator>>(vec3 b) const { return {T(x>>b.x), T(y>>b.y), T(z>>b.z)}; }
    vec3 operator<<(int b) const { return {T(x<<b), T(y<<b), T(z<<b)}; }
    vec3 operator>>(int b) const { return {T(x>>b), T(y>>b), T(z>>b)}; }
    vec3& operator%=(vec3 b) { x%=b.x; y%=b.y; z%=b.z; return *this; }
    vec3& operator&=(vec3 b) { x&=b.x; y&=b.y; z&=b.z; return *this; }
    vec3& operator|=(vec3 b) { x|=b.x; y|=b.y; z|=b.z; return *this; }
    vec3& operator^=(vec3 b) { x^=b.x; y^=b.y; z^=b.z; return *this; }
    vec3& operator<<=(vec3 b) { x<<=b.x; y<<=b.y; z<<=b.z; return *this; }
    vec3& operator>>=(vec3 b) { x>>=b.x; y>>=b.y; z>>=b.z; return *this; }
    vec3& operator<<=(int b) { x<<=b; y<<=b; z<<=b; return *this; }
    vec3& operator>>=(int b) { x>>=b; y>>=b; z>>=b; return *this; }
    bool operator!() const { return !x && !y && !z; }
};

template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec3<T> operator*(S a, vec3<T> b) { return {T(a*b.x), T(a*b.y), T(a*b.z)}; }
template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec3<T> operator+(S a, vec3<T> b) { return {T(a+b.x), T(a+b.y), T(a+b.z)}; }
template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec3<T> operator-(S a, vec3<T> b) { return {T(a-b.x), T(a-b.y), T(a-b.z)}; }
template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec3<T> operator/(S a, vec3<T> b) { return {T(a/b.x), T(a/b.y), T(a/b.z)}; }

// ============================================================================
// vec4<T>
// ============================================================================

template <typename T>
struct vec4 {
    union {
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
    };
    // Swizzle properties — overlay same memory, for clangd autocomplete
    vec2<T>& xy;
    vec3<T>& xyz;
    vec3<T>& rgb;

    vec4() : x{}, y{}, z{}, w{}, xy(*(vec2<T>*)this), xyz(*(vec3<T>*)this), rgb(*(vec3<T>*)this) {}
    /// Construct from scalar (broadcast)
    vec4(T v) : x(v), y(v), z(v), w(v), xy(*(vec2<T>*)this), xyz(*(vec3<T>*)this), rgb(*(vec3<T>*)this) {}
    /// Construct from components
    vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w), xy(*(vec2<T>*)this), xyz(*(vec3<T>*)this), rgb(*(vec3<T>*)this) {}
    /// Construct from vec2 + vec2
    vec4(vec2<T> a, vec2<T> b) : x(a.x), y(a.y), z(b.x), w(b.y), xy(*(vec2<T>*)this), xyz(*(vec3<T>*)this), rgb(*(vec3<T>*)this) {}
    /// Construct from vec3 + scalar
    vec4(vec3<T> v, T w) : x(v.x), y(v.y), z(v.z), w(w), xy(*(vec2<T>*)this), xyz(*(vec3<T>*)this), rgb(*(vec3<T>*)this) {}
    /// Construct from scalar + vec3
    vec4(T x, vec3<T> v) : x(x), y(v.x), z(v.y), w(v.z), xy(*(vec2<T>*)this), xyz(*(vec3<T>*)this), rgb(*(vec3<T>*)this) {}
    /// Construct from vec2 + scalar + scalar
    vec4(vec2<T> v, T z, T w) : x(v.x), y(v.y), z(z), w(w), xy(*(vec2<T>*)this), xyz(*(vec3<T>*)this), rgb(*(vec3<T>*)this) {}
    /// Construct from scalar + scalar + vec2
    vec4(T x, T y, vec2<T> v) : x(x), y(y), z(v.x), w(v.y), xy(*(vec2<T>*)this), xyz(*(vec3<T>*)this), rgb(*(vec3<T>*)this) {}
    /// Convert from another element type
    template <typename U>
    explicit vec4(vec4<U> v) : x(T(v.x)), y(T(v.y)), z(T(v.z)), w(T(v.w)), xy(*(vec2<T>*)this), xyz(*(vec3<T>*)this), rgb(*(vec3<T>*)this) {}
    /// Copy
    vec4(const vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w), xy(*(vec2<T>*)this), xyz(*(vec3<T>*)this), rgb(*(vec3<T>*)this) {}
    /// Copy from volatile (needed for device→volatile mapping)
    vec4(const volatile vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w), xy(*(vec2<T>*)this), xyz(*(vec3<T>*)this), rgb(*(vec3<T>*)this) {}
    vec4& operator=(const vec4& v) { x=v.x; y=v.y; z=v.z; w=v.w; return *this; }
    // Volatile-qualified assignment (needed for thread→volatile mapping)
    volatile vec4& operator=(const vec4& v) volatile { x=v.x; y=v.y; z=v.z; w=v.w; return *this; }

    T operator[](int i) const { return (&x)[i]; }
    T& operator[](int i) { return (&x)[i]; }

    // Additional swizzle access
    vec2<T> get_xz() const { return {x, z}; }
    vec2<T> get_yz() const { return {y, z}; }
    vec2<T> get_xw() const { return {x, w}; }
    vec2<T> get_yw() const { return {y, w}; }
    vec2<T> get_zw() const { return {z, w}; }
    vec3<T> get_xyw() const { return {x, y, w}; }
    vec3<T> get_xzw() const { return {x, z, w}; }
    vec3<T> get_yzw() const { return {y, z, w}; }

    vec4 operator+(vec4 b) const { return {T(x+b.x), T(y+b.y), T(z+b.z), T(w+b.w)}; }
    vec4 operator-(vec4 b) const { return {T(x-b.x), T(y-b.y), T(z-b.z), T(w-b.w)}; }
    vec4 operator*(vec4 b) const { return {T(x*b.x), T(y*b.y), T(z*b.z), T(w*b.w)}; }
    vec4 operator/(vec4 b) const { return {T(x/b.x), T(y/b.y), T(z/b.z), T(w/b.w)}; }
    vec4 operator+(T b) const { return {T(x+b), T(y+b), T(z+b), T(w+b)}; }
    vec4 operator-(T b) const { return {T(x-b), T(y-b), T(z-b), T(w-b)}; }
    vec4 operator*(T b) const { return {T(x*b), T(y*b), T(z*b), T(w*b)}; }
    vec4 operator/(T b) const { return {T(x/b), T(y/b), T(z/b), T(w/b)}; }
    // Volatile-qualified arithmetic (for device pointer access)
    vec4 operator+(vec4 b) const volatile { return {T(x+b.x), T(y+b.y), T(z+b.z), T(w+b.w)}; }
    vec4 operator-(vec4 b) const volatile { return {T(x-b.x), T(y-b.y), T(z-b.z), T(w-b.w)}; }
    vec4 operator*(vec4 b) const volatile { return {T(x*b.x), T(y*b.y), T(z*b.z), T(w*b.w)}; }
    vec4 operator/(vec4 b) const volatile { return {T(x/b.x), T(y/b.y), T(z/b.z), T(w/b.w)}; }
    vec4 operator+(T b) const volatile { return {T(x+b), T(y+b), T(z+b), T(w+b)}; }
    vec4 operator-(T b) const volatile { return {T(x-b), T(y-b), T(z-b), T(w-b)}; }
    vec4 operator*(T b) const volatile { return {T(x*b), T(y*b), T(z*b), T(w*b)}; }
    vec4 operator/(T b) const volatile { return {T(x/b), T(y/b), T(z/b), T(w/b)}; }
    vec4 operator-() const volatile { return {T(-x), T(-y), T(-z), T(-w)}; }
    vec4& operator+=(vec4 b) { x+=b.x; y+=b.y; z+=b.z; w+=b.w; return *this; }
    vec4& operator-=(vec4 b) { x-=b.x; y-=b.y; z-=b.z; w-=b.w; return *this; }
    vec4& operator*=(vec4 b) { x*=b.x; y*=b.y; z*=b.z; w*=b.w; return *this; }
    vec4& operator/=(vec4 b) { x/=b.x; y/=b.y; z/=b.z; w/=b.w; return *this; }
    vec4& operator+=(T b) { x+=b; y+=b; z+=b; w+=b; return *this; }
    vec4& operator-=(T b) { x-=b; y-=b; z-=b; w-=b; return *this; }
    vec4& operator*=(T b) { x*=b; y*=b; z*=b; w*=b; return *this; }
    vec4& operator/=(T b) { x/=b; y/=b; z/=b; w/=b; return *this; }
    vec4 operator-() const { return {T(-x), T(-y), T(-z), T(-w)}; }
    bool operator==(vec4 b) const { return x==b.x && y==b.y && z==b.z && w==b.w; }
    bool operator!=(vec4 b) const { return !(*this == b); }
    bool operator<(vec4 b) const { return x<b.x && y<b.y && z<b.z && w<b.w; }
    bool operator>(vec4 b) const { return x>b.x && y>b.y && z>b.z && w>b.w; }
    bool operator<=(vec4 b) const { return x<=b.x && y<=b.y && z<=b.z && w<=b.w; }
    bool operator>=(vec4 b) const { return x>=b.x && y>=b.y && z>=b.z && w>=b.w; }

    vec4 operator%(vec4 b) const { return {T(x%b.x), T(y%b.y), T(z%b.z), T(w%b.w)}; }
    vec4 operator&(vec4 b) const { return {T(x&b.x), T(y&b.y), T(z&b.z), T(w&b.w)}; }
    vec4 operator|(vec4 b) const { return {T(x|b.x), T(y|b.y), T(z|b.z), T(w|b.w)}; }
    vec4 operator^(vec4 b) const { return {T(x^b.x), T(y^b.y), T(z^b.z), T(w^b.w)}; }
    vec4 operator~() const { return {T(~x), T(~y), T(~z), T(~w)}; }
    vec4 operator<<(vec4 b) const { return {T(x<<b.x), T(y<<b.y), T(z<<b.z), T(w<<b.w)}; }
    vec4 operator>>(vec4 b) const { return {T(x>>b.x), T(y>>b.y), T(z>>b.z), T(w>>b.w)}; }
    vec4 operator<<(int b) const { return {T(x<<b), T(y<<b), T(z<<b), T(w<<b)}; }
    vec4 operator>>(int b) const { return {T(x>>b), T(y>>b), T(z>>b), T(w>>b)}; }
    vec4& operator%=(vec4 b) { x%=b.x; y%=b.y; z%=b.z; w%=b.w; return *this; }
    vec4& operator&=(vec4 b) { x&=b.x; y&=b.y; z&=b.z; w&=b.w; return *this; }
    vec4& operator|=(vec4 b) { x|=b.x; y|=b.y; z|=b.z; w|=b.w; return *this; }
    vec4& operator^=(vec4 b) { x^=b.x; y^=b.y; z^=b.z; w^=b.w; return *this; }
    vec4& operator<<=(vec4 b) { x<<=b.x; y<<=b.y; z<<=b.z; w<<=b.w; return *this; }
    vec4& operator>>=(vec4 b) { x>>=b.x; y>>=b.y; z>>=b.z; w>>=b.w; return *this; }
    vec4& operator<<=(int b) { x<<=b; y<<=b; z<<=b; w<<=b; return *this; }
    vec4& operator>>=(int b) { x>>=b; y>>=b; z>>=b; w>>=b; return *this; }
    bool operator!() const { return !x && !y && !z && !w; }
};

template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec4<T> operator*(S a, vec4<T> b) { return {T(a*b.x), T(a*b.y), T(a*b.z), T(a*b.w)}; }
template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec4<T> operator+(S a, vec4<T> b) { return {T(a+b.x), T(a+b.y), T(a+b.z), T(a+b.w)}; }
template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec4<T> operator-(S a, vec4<T> b) { return {T(a-b.x), T(a-b.y), T(a-b.z), T(a-b.w)}; }
template <typename S, typename T, typename = typename _enable_if<!__is_class(S)>::type> vec4<T> operator/(S a, vec4<T> b) { return {T(a/b.x), T(a/b.y), T(a/b.z), T(a/b.w)}; }

// ============================================================================
// Named vector type aliases
// ============================================================================

// float vectors
typedef vec2<float> float2;
typedef vec3<float> float3;
typedef vec4<float> float4;

// half vectors
typedef vec2<half> half2;
typedef vec3<half> half3;
typedef vec4<half> half4;

// int vectors
typedef vec2<int> int2;
typedef vec3<int> int3;
typedef vec4<int> int4;

// uint vectors
typedef vec2<uint> uint2;
typedef vec3<uint> uint3;
typedef vec4<uint> uint4;

// short vectors
typedef vec2<short> short2;
typedef vec3<short> short3;
typedef vec4<short> short4;

// ushort vectors
typedef vec2<ushort> ushort2;
typedef vec3<ushort> ushort3;
typedef vec4<ushort> ushort4;

// char vectors
typedef vec2<char> char2;
typedef vec3<char> char3;
typedef vec4<char> char4;

// uchar vectors
typedef vec2<uchar> uchar2;
typedef vec3<uchar> uchar3;
typedef vec4<uchar> uchar4;

// bool vectors
typedef vec2<bool> bool2;
typedef vec3<bool> bool3;
typedef vec4<bool> bool4;

// long vectors
typedef vec2<long> long2;
typedef vec3<long> long3;
typedef vec4<long> long4;

// ulong vectors
typedef vec2<ulong> ulong2;
typedef vec3<ulong> ulong3;
typedef vec4<ulong> ulong4;

// ============================================================================
// Packed vector types (3-component, tightly packed — no padding)
// ============================================================================

/// Packed 3-component float vector (12 bytes, no padding)
struct packed_float3 {
    float x, y, z;
    packed_float3() = default;
    packed_float3(float x, float y, float z) : x(x), y(y), z(z) {}
    packed_float3(float3 v) : x(v.x), y(v.y), z(v.z) {}
    operator float3() const { return float3(x, y, z); }
    operator float3() const volatile { return float3(x, y, z); }
};

/// Packed 3-component half vector (6 bytes, no padding)
struct packed_half3 {
    half x, y, z;
    packed_half3() = default;
    packed_half3(half x, half y, half z) : x(x), y(y), z(z) {}
    packed_half3(half3 v) : x(v.x), y(v.y), z(v.z) {}
    operator half3() const { return half3(x, y, z); }
    operator half3() const volatile { return half3(x, y, z); }
};

/// Packed 3-component int vector (12 bytes, no padding)
struct packed_int3 {
    int x, y, z;
    packed_int3() = default;
    packed_int3(int x, int y, int z) : x(x), y(y), z(z) {}
    packed_int3(int3 v) : x(v.x), y(v.y), z(v.z) {}
    operator int3() const { return int3(x, y, z); }
};

/// Packed 3-component uint vector (12 bytes, no padding)
struct packed_uint3 {
    uint x, y, z;
    packed_uint3() = default;
    packed_uint3(uint x, uint y, uint z) : x(x), y(y), z(z) {}
    packed_uint3(uint3 v) : x(v.x), y(v.y), z(v.z) {}
    operator uint3() const { return uint3(x, y, z); }
};

/// Packed 3-component short vector (6 bytes, no padding)
struct packed_short3 {
    short x, y, z;
    packed_short3() = default;
    packed_short3(short x, short y, short z) : x(x), y(y), z(z) {}
    packed_short3(short3 v) : x(v.x), y(v.y), z(v.z) {}
    operator short3() const { return short3(x, y, z); }
};

/// Packed 3-component ushort vector (6 bytes, no padding)
struct packed_ushort3 {
    ushort x, y, z;
    packed_ushort3() = default;
    packed_ushort3(ushort x, ushort y, ushort z) : x(x), y(y), z(z) {}
    packed_ushort3(ushort3 v) : x(v.x), y(v.y), z(v.z) {}
    operator ushort3() const { return ushort3(x, y, z); }
};

/// Packed 3-component char vector (3 bytes, no padding)
struct packed_char3 {
    char x, y, z;
    packed_char3() = default;
    packed_char3(char x, char y, char z) : x(x), y(y), z(z) {}
    packed_char3(char3 v) : x(v.x), y(v.y), z(v.z) {}
    operator char3() const { return char3(x, y, z); }
};

/// Packed 3-component uchar vector (3 bytes, no padding)
struct packed_uchar3 {
    uchar x, y, z;
    packed_uchar3() = default;
    packed_uchar3(uchar x, uchar y, uchar z) : x(x), y(y), z(z) {}
    packed_uchar3(uchar3 v) : x(v.x), y(v.y), z(v.z) {}
    operator uchar3() const { return uchar3(x, y, z); }
};

/// Packed 2-component float vector
struct packed_float2 {
    float x, y;
    packed_float2() = default;
    packed_float2(float x, float y) : x(x), y(y) {}
    packed_float2(float2 v) : x(v.x), y(v.y) {}
    operator float2() const { return float2(x, y); }
    operator float2() const volatile { return float2(x, y); }
};

/// Packed 4-component float vector
struct packed_float4 {
    float x, y, z, w;
    packed_float4() = default;
    packed_float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    packed_float4(float4 v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
    operator float4() const { return float4(x, y, z, w); }
    operator float4() const volatile { return float4(x, y, z, w); }
};

// ============================================================================
// Fixed-size array — used in argument buffers and general Metal code
// ============================================================================

/// Fixed-size array type for use in argument buffers and structs
template <typename T, unsigned N>
struct array {
    T elements[N];

    thread T& operator[](unsigned i) { return elements[i]; }
    const thread T& operator[](unsigned i) const { return elements[i]; }
    volatile T& operator[](unsigned i) volatile { return elements[i]; }
    const volatile T& operator[](unsigned i) const volatile { return elements[i]; }

    constexpr unsigned size() const { return N; }
};

} // namespace metal
