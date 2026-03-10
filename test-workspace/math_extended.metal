#include <metal_stdlib>
using namespace metal;

// =============================================================================
// Extended math functions — pi-multiplied trig, gamma, error functions, etc.
// =============================================================================

kernel void pi_trig_functions(
    device float4* output [[buffer(0)]],
    uint tid [[thread_position_in_grid]])
{
    float x = float(tid) / 1024.0f;

    // Pi-multiplied trigonometric functions
    float s = sinpi(x);         // sin(pi * x)
    float c = cospi(x);         // cos(pi * x)
    float t = tanpi(x * 0.25f); // tan(pi * x/4)

    // Inverse pi-multiplied trig
    float as = asinpi(x);    // asin(x) / pi
    float ac = acospi(x);    // acos(x) / pi
    float at = atanpi(x);    // atan(x) / pi
    float at2 = atan2pi(s, c); // atan2(y, x) / pi

    output[tid] = float4(s + c + t, as + ac, at + at2, 0.0f);

    // Vector variants
    float3 v = float3(x, x * 0.5f, x * 0.25f);
    float3 sv = sinpi(v);
    float3 cv = cospi(v);
    float3 tv = tanpi(v * 0.1f);
    float3 asv = asinpi(v * 0.5f);
    float3 acv = acospi(v * 0.5f);
    float3 atv = atanpi(v);
    float3 at2v = atan2pi(sv, cv);
    output[tid] += float4(sv.x + cv.x + tv.x + asv.x + acv.x + atv.x + at2v.x, 0, 0, 0);
}

kernel void extended_exp_log(
    device float4* output [[buffer(0)]],
    uint tid [[thread_position_in_grid]])
{
    float x = float(tid) / 1024.0f + 0.001f;

    // exp10, expm1, log1p
    float e10 = exp10(x);        // 10^x
    float em1 = expm1(x);        // e^x - 1 (precise for small x)
    float l1p = log1p(x);        // log(1 + x) (precise for small x)

    output[tid] = float4(e10, em1, l1p, 0.0f);

    // Vector variants
    float3 v = float3(x, x * 2.0f, x * 3.0f);
    float3 e10v = exp10(v);
    float3 em1v = expm1(v);
    float3 l1pv = log1p(v);
    output[tid] += float4(e10v.x + em1v.x + l1pv.x, 0, 0, 0);
}

kernel void extended_power_root(
    device float4* output [[buffer(0)]],
    uint tid [[thread_position_in_grid]])
{
    float x = float(tid) / 1024.0f + 1.0f;
    float y = float(tid) / 2048.0f + 0.5f;

    // cbrt, hypot, pown, rootn
    float cr = cbrt(x);            // cube root
    float h = hypot(x, y);         // sqrt(x*x + y*y)
    float pn = pown(x, 3);         // x^3 (integer exponent)
    float rn = rootn(x, 3);        // x^(1/3) (integer root)

    output[tid] = float4(cr, h, pn, rn);

    // Vector variants for cbrt and hypot
    float3 v1 = float3(x, x * 2.0f, x * 3.0f);
    float3 v2 = float3(y, y * 2.0f, y * 3.0f);
    float3 crv = cbrt(v1);
    float3 hv = hypot(v1, v2);
    output[tid] += float4(crv.x + hv.x, 0, 0, 0);
}

kernel void remainder_rounding(
    device float4* output [[buffer(0)]],
    uint tid [[thread_position_in_grid]])
{
    float x = float(tid) / 100.0f;
    float y = 3.14f;

    // remainder, rint, copysign, fdim
    float rem = remainder(x, y);     // IEEE 754 remainder
    float ri = rint(x);             // round to nearest even
    float cs = copysign(x, -1.0f);  // copy sign
    float fd = fdim(x, y);          // max(x - y, 0)

    output[tid] = float4(rem, ri, cs, fd);

    // Vector variants
    float3 v1 = float3(x, x * 2.0f, x * 3.0f);
    float3 v2 = float3(y, y * 0.5f, y * 2.0f);
    float3 remv = remainder(v1, v2);
    float3 riv = rint(v1);
    float3 csv = copysign(v1, float3(-1.0f));
    float3 fdv = fdim(v1, v2);
    output[tid] += float4(remv.x + riv.x + csv.x + fdv.x, 0, 0, 0);
}

kernel void gamma_error_functions(
    device float4* output [[buffer(0)]],
    uint tid [[thread_position_in_grid]])
{
    float x = float(tid) / 1024.0f + 0.5f;

    // Gamma functions
    float lg = lgamma(x);    // log-gamma
    float tg = tgamma(x);    // gamma function

    // Error functions
    float e = erf(x);        // error function
    float ec = erfc(x);      // complementary error function (1 - erf)

    output[tid] = float4(lg, tg, e, ec);

    // Vector variants
    float3 v = float3(x, x + 1.0f, x + 2.0f);
    float3 lgv = lgamma(v);
    float3 tgv = tgamma(v);
    float3 ev = erf(v);
    float3 ecv = erfc(v);
    output[tid] += float4(lgv.x + tgv.x + ev.x + ecv.x, 0, 0, 0);
}

// Half-precision variants
kernel void half_precision_math(
    device half4* output [[buffer(0)]],
    uint tid [[thread_position_in_grid]])
{
    half x = half(tid) / 1024.0h;

    // Pi-multiplied trig (half)
    half s = sinpi(x);
    half c = cospi(x);
    half t = tanpi(x * 0.25h);

    // Extended functions (half)
    half e10 = exp10(x + 1.0h);
    half em1 = expm1(x);
    half l1p = log1p(x);
    half cr = cbrt(x + 1.0h);

    output[tid] = half4(s + c + t, e10, em1 + l1p, cr);

    // Half vector variants
    half3 v = half3(x, x * 0.5h, x * 0.25h);
    half3 sv = sinpi(v);
    half3 cv = cospi(v);
    output[tid] += half4(sv.x + cv.x, 0.0h, 0.0h, 0.0h);
}
