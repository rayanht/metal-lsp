#pragma once
#include <metal_types.h>

namespace metal {

// ============================================================================
// Integer functions
// ============================================================================

/// Absolute value of integer x
int abs(int x);
int2 abs(int2 x);
int3 abs(int3 x);
int4 abs(int4 x);
short abs(short x);
short2 abs(short2 x);
short3 abs(short3 x);
short4 abs(short4 x);
char abs(char x);
char2 abs(char2 x);
char3 abs(char3 x);
char4 abs(char4 x);

/// Count leading zeros
uint clz(uint x);
uint2 clz(uint2 x);
uint3 clz(uint3 x);
uint4 clz(uint4 x);
int clz(int x);
int2 clz(int2 x);
int3 clz(int3 x);
int4 clz(int4 x);
ushort clz(ushort x);
ushort2 clz(ushort2 x);
ushort3 clz(ushort3 x);
ushort4 clz(ushort4 x);
short clz(short x);
short2 clz(short2 x);
short3 clz(short3 x);
short4 clz(short4 x);
uchar clz(uchar x);
uchar2 clz(uchar2 x);
uchar3 clz(uchar3 x);
uchar4 clz(uchar4 x);

/// Count trailing zeros
uint ctz(uint x);
uint2 ctz(uint2 x);
uint3 ctz(uint3 x);
uint4 ctz(uint4 x);
int ctz(int x);
int2 ctz(int2 x);
int3 ctz(int3 x);
int4 ctz(int4 x);
ushort ctz(ushort x);
ushort2 ctz(ushort2 x);
ushort3 ctz(ushort3 x);
ushort4 ctz(ushort4 x);
short ctz(short x);
short2 ctz(short2 x);
short3 ctz(short3 x);
short4 ctz(short4 x);

/// Extract a bit field from x, starting at offset with width bits
uint extract_bits(uint x, uint offset, uint width);
uint2 extract_bits(uint2 x, uint offset, uint width);
uint3 extract_bits(uint3 x, uint offset, uint width);
uint4 extract_bits(uint4 x, uint offset, uint width);
int extract_bits(int x, uint offset, uint width);
int2 extract_bits(int2 x, uint offset, uint width);
int3 extract_bits(int3 x, uint offset, uint width);
int4 extract_bits(int4 x, uint offset, uint width);
ushort extract_bits(ushort x, uint offset, uint width);
short extract_bits(short x, uint offset, uint width);
ulong extract_bits(ulong x, uint offset, uint width);
unsigned long long extract_bits(unsigned long long x, uint offset, uint width);

/// Insert bits from insert into x at offset with width
uint insert_bits(uint x, uint insert, uint offset, uint width);
uint2 insert_bits(uint2 x, uint2 insert, uint offset, uint width);
uint3 insert_bits(uint3 x, uint3 insert, uint offset, uint width);
uint4 insert_bits(uint4 x, uint4 insert, uint offset, uint width);
int insert_bits(int x, int insert, uint offset, uint width);
int2 insert_bits(int2 x, int2 insert, uint offset, uint width);
int3 insert_bits(int3 x, int3 insert, uint offset, uint width);
int4 insert_bits(int4 x, int4 insert, uint offset, uint width);
ushort insert_bits(ushort x, ushort insert, uint offset, uint width);
short insert_bits(short x, short insert, uint offset, uint width);

/// Count the number of set bits (population count)
uint popcount(uint x);
uint2 popcount(uint2 x);
uint3 popcount(uint3 x);
uint4 popcount(uint4 x);
int popcount(int x);
int2 popcount(int2 x);
int3 popcount(int3 x);
int4 popcount(int4 x);
ushort popcount(ushort x);
ushort2 popcount(ushort2 x);
ushort3 popcount(ushort3 x);
ushort4 popcount(ushort4 x);
short popcount(short x);
short2 popcount(short2 x);
short3 popcount(short3 x);
short4 popcount(short4 x);
uchar popcount(uchar x);
uchar2 popcount(uchar2 x);

/// Reverse all bits in x
uint reverse_bits(uint x);
uint2 reverse_bits(uint2 x);
uint3 reverse_bits(uint3 x);
uint4 reverse_bits(uint4 x);
int reverse_bits(int x);
int2 reverse_bits(int2 x);
int3 reverse_bits(int3 x);
int4 reverse_bits(int4 x);
ushort reverse_bits(ushort x);
ushort2 reverse_bits(ushort2 x);
short reverse_bits(short x);
short2 reverse_bits(short2 x);

/// Rotate x left by amount bits
uint rotate(uint x, uint amount);
uint2 rotate(uint2 x, uint2 amount);
uint3 rotate(uint3 x, uint3 amount);
uint4 rotate(uint4 x, uint4 amount);
int rotate(int x, int amount);
int2 rotate(int2 x, int2 amount);
int3 rotate(int3 x, int3 amount);
int4 rotate(int4 x, int4 amount);
ushort rotate(ushort x, ushort amount);
ushort2 rotate(ushort2 x, ushort2 amount);
short rotate(short x, short amount);
short2 rotate(short2 x, short2 amount);

/// Return the maximum of x and y
int max(int x, int y);
int2 max(int2 x, int2 y);
int3 max(int3 x, int3 y);
int4 max(int4 x, int4 y);
uint max(uint x, uint y);
uint2 max(uint2 x, uint2 y);
uint3 max(uint3 x, uint3 y);
uint4 max(uint4 x, uint4 y);
short max(short x, short y);
short2 max(short2 x, short2 y);
ushort max(ushort x, ushort y);
ushort2 max(ushort2 x, ushort2 y);

/// Return the minimum of x and y
int min(int x, int y);
int2 min(int2 x, int2 y);
int3 min(int3 x, int3 y);
int4 min(int4 x, int4 y);
uint min(uint x, uint y);
uint2 min(uint2 x, uint2 y);
uint3 min(uint3 x, uint3 y);
uint4 min(uint4 x, uint4 y);
short min(short x, short y);
short2 min(short2 x, short2 y);
ushort min(ushort x, ushort y);
ushort2 min(ushort2 x, ushort2 y);

/// Clamp x to the range [minval, maxval]
int clamp(int x, int minval, int maxval);
int2 clamp(int2 x, int2 minval, int2 maxval);
int3 clamp(int3 x, int3 minval, int3 maxval);
int4 clamp(int4 x, int4 minval, int4 maxval);
uint clamp(uint x, uint minval, uint maxval);
uint2 clamp(uint2 x, uint2 minval, uint2 maxval);
uint3 clamp(uint3 x, uint3 minval, uint3 maxval);
uint4 clamp(uint4 x, uint4 minval, uint4 maxval);
short clamp(short x, short minval, short maxval);
short2 clamp(short2 x, short2 minval, short2 maxval);
ushort clamp(ushort x, ushort minval, ushort maxval);
ushort2 clamp(ushort2 x, ushort2 minval, ushort2 maxval);

/// Multiply x and y, returning high bits of the result
int mulhi(int x, int y);
uint mulhi(uint x, uint y);
short mulhi(short x, short y);
ushort mulhi(ushort x, ushort y);

/// Add with saturation (clamp to type range instead of wrapping)
int add_sat(int x, int y);
uint add_sat(uint x, uint y);
short add_sat(short x, short y);
ushort add_sat(ushort x, ushort y);
char add_sat(char x, char y);
uchar add_sat(uchar x, uchar y);

/// Subtract with saturation
int sub_sat(int x, int y);
uint sub_sat(uint x, uint y);
short sub_sat(short x, short y);
ushort sub_sat(ushort x, ushort y);
char sub_sat(char x, char y);
uchar sub_sat(uchar x, uchar y);

} // namespace metal
