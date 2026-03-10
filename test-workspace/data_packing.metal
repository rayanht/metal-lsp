#include <metal_stdlib>
using namespace metal;

// =============================================================================
// Data packing/unpacking — compress float/half to normalized integer formats
// =============================================================================

// Typical use case: vertex attribute compression in a mesh processing kernel
kernel void pack_vertex_normals(
    device const float3* normals [[buffer(0)]],
    device uint* packed_normals [[buffer(1)]],
    uint tid [[thread_position_in_grid]])
{
    float3 n = normalize(normals[tid]);

    // Pack normal XYZ into snorm4x8 (4 signed bytes, W=0)
    float4 n4 = float4(n, 0.0f);
    packed_normals[tid] = pack_float_to_snorm4x8(n4);
}

kernel void unpack_vertex_normals(
    device const uint* packed_normals [[buffer(0)]],
    device float3* normals [[buffer(1)]],
    uint tid [[thread_position_in_grid]])
{
    float4 n4 = unpack_snorm4x8_to_float(packed_normals[tid]);
    normals[tid] = normalize(n4.xyz);
}

// UV coordinate packing — common for texture coordinates
kernel void pack_uvs(
    device const float2* uvs [[buffer(0)]],
    device uint* packed_uvs [[buffer(1)]],
    uint tid [[thread_position_in_grid]])
{
    // UVs are [0,1] — pack into unorm 2x16 (two unsigned shorts)
    packed_uvs[tid] = pack_float_to_unorm2x16(uvs[tid]);
}

kernel void unpack_uvs(
    device const uint* packed_uvs [[buffer(0)]],
    device float2* uvs [[buffer(1)]],
    uint tid [[thread_position_in_grid]])
{
    uvs[tid] = unpack_unorm2x16_to_float(packed_uvs[tid]);
}

// Per-component packing to short/char — used in vertex buffers and G-buffers
kernel void pack_gbuffer_data(
    device const float3* normals [[buffer(0)]],
    device const float4* colors [[buffer(1)]],
    device short4* packed_normals [[buffer(2)]],
    device uchar4* packed_colors [[buffer(3)]],
    uint tid [[thread_position_in_grid]])
{
    // Pack normals to snorm short (16-bit precision per component)
    float3 n = normals[tid];
    short4 pn;
    pn.xyz = pack_float_to_snorm_short4(float4(n, 0.0f)).xyz;
    pn.w = 0;
    packed_normals[tid] = pn;

    // Pack colors to unorm uchar (8-bit per channel — standard RGBA8)
    packed_colors[tid] = pack_float_to_unorm_uchar4(colors[tid]);
}

kernel void unpack_gbuffer_data(
    device const short4* packed_normals [[buffer(0)]],
    device const uchar4* packed_colors [[buffer(1)]],
    device float3* normals [[buffer(2)]],
    device float4* colors [[buffer(3)]],
    uint tid [[thread_position_in_grid]])
{
    // Unpack normals from snorm short
    float4 n4 = unpack_snorm_short4_to_float(packed_normals[tid]);
    normals[tid] = normalize(n4.xyz);

    // Unpack colors from unorm uchar
    colors[tid] = unpack_unorm_uchar4_to_float(packed_colors[tid]);
}

// HDR color packing with ushort — higher precision than uchar
kernel void pack_hdr_colors(
    device const float4* hdr_colors [[buffer(0)]],
    device ushort4* packed [[buffer(1)]],
    uint tid [[thread_position_in_grid]])
{
    // Tonemap to [0,1] then pack to unorm ushort (16-bit per channel)
    float4 c = hdr_colors[tid];
    float4 tonemapped = c / (c + 1.0f); // Reinhard tonemap
    packed[tid] = pack_float_to_unorm_ushort4(tonemapped);
}

kernel void unpack_hdr_colors(
    device const ushort4* packed [[buffer(0)]],
    device float4* hdr_colors [[buffer(1)]],
    uint tid [[thread_position_in_grid]])
{
    // Unpack and invert tonemap
    float4 c = unpack_unorm_ushort4_to_float(packed[tid]);
    hdr_colors[tid] = c / max(1.0f - c, 0.001f); // Inverse Reinhard
}

// Half-precision packing — used when input data is already half
kernel void pack_half_normals(
    device const half3* normals [[buffer(0)]],
    device short4* packed [[buffer(1)]],
    uint tid [[thread_position_in_grid]])
{
    half3 n = normalize(normals[tid]);
    packed[tid] = pack_half_to_snorm_short4(half4(n, 0.0h));
}

kernel void unpack_half_normals(
    device const short4* packed [[buffer(0)]],
    device half3* normals [[buffer(1)]],
    uint tid [[thread_position_in_grid]])
{
    half4 n4 = unpack_snorm_short4_to_half(packed[tid]);
    normals[tid] = normalize(n4.xyz);
}

// Scalar packing — individual component packing
kernel void scalar_packing_test(
    device float* output [[buffer(0)]],
    uint tid [[thread_position_in_grid]])
{
    float x = float(tid) / 1024.0f;

    // Float → snorm short → float roundtrip
    short s = pack_float_to_snorm_short(x * 2.0f - 1.0f);
    float roundtrip_s = unpack_snorm_short_to_float(s);

    // Float → unorm ushort → float roundtrip
    ushort u = pack_float_to_unorm_ushort(x);
    float roundtrip_u = unpack_unorm_ushort_to_float(u);

    // Float → snorm char → float roundtrip
    char c = pack_float_to_snorm_char(x * 2.0f - 1.0f);
    float roundtrip_c = unpack_snorm_char_to_float(c);

    // Float → unorm uchar → float roundtrip
    uchar uc = pack_float_to_unorm_uchar(x);
    float roundtrip_uc = unpack_unorm_uchar_to_float(uc);

    output[tid] = roundtrip_s + roundtrip_u + roundtrip_c + roundtrip_uc;
}

// 2x16 and 4x8 snorm packing — common for compressed tangent frames
kernel void tangent_frame_packing(
    device const float3* tangents [[buffer(0)]],
    device const float3* bitangents [[buffer(1)]],
    device uint2* packed_frame [[buffer(2)]],
    uint tid [[thread_position_in_grid]])
{
    float3 T = normalize(tangents[tid]);
    float3 B = normalize(bitangents[tid]);

    // Pack tangent XY and bitangent XY into snorm 2x16
    packed_frame[tid].x = pack_float_to_snorm2x16(T.xy);
    packed_frame[tid].y = pack_float_to_snorm2x16(B.xy);
}

kernel void tangent_frame_unpacking(
    device const uint2* packed_frame [[buffer(0)]],
    device float3* tangents [[buffer(1)]],
    device float3* bitangents [[buffer(2)]],
    uint tid [[thread_position_in_grid]])
{
    // Unpack and reconstruct Z from unit-length constraint
    float2 Txy = unpack_snorm2x16_to_float(packed_frame[tid].x);
    float2 Bxy = unpack_snorm2x16_to_float(packed_frame[tid].y);

    float Tz = sqrt(max(1.0f - dot(Txy, Txy), 0.0f));
    float Bz = sqrt(max(1.0f - dot(Bxy, Bxy), 0.0f));

    tangents[tid] = float3(Txy, Tz);
    bitangents[tid] = float3(Bxy, Bz);
}

// Half ↔ uint bit-preserving conversion
kernel void half_uint_bitcast(
    device const half2* input [[buffer(0)]],
    device uint* packed [[buffer(1)]],
    device half2* unpacked [[buffer(2)]],
    uint tid [[thread_position_in_grid]])
{
    // Pack two halfs into one uint
    packed[tid] = as_uint(input[tid]);

    // Unpack back
    unpacked[tid] = as_half2(packed[tid]);
}
