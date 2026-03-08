#include <metal_stdlib>
using namespace metal;

#define TILE_PIXELS 64
#define SSIM_TG 8

kernel void test_operators(
    device const float* ssim_h_buf [[buffer(0)]],
    uint tr [[thread_index_in_threadgroup]],
    uint2 tg_id [[threadgroup_position_in_grid]]
) {
    threadgroup float tg_h1[3][SSIM_TG][SSIM_TG];
    threadgroup float tg_h2[3][SSIM_TG][SSIM_TG];
    threadgroup float tg_h3[3][SSIM_TG][SSIM_TG];

    uint W = 1920;
    uint H = 1080;
    int base_gy = 0;
    int base_gx = 0;

    for (uint c = 0; c < 3; c++) {
        for (uint i = tr; i < TILE_PIXELS; i += SSIM_TG * SSIM_TG) {
            uint sy = i / SSIM_TG;
            uint sx = i % SSIM_TG;
            int gy = base_gy + (int)sy;
            int gx = base_gx + (int)sx;
            float v1 = 0, v2 = 0, v3 = 0;
            if (gx >= 0 && gx < (int)W && gy >= 0 && gy < (int)H) {
                uint hp = (gy * W + gx) * 15 + c * 5;
                v1 = ssim_h_buf[hp + 0];
                v2 = ssim_h_buf[hp + 1];
                v3 = ssim_h_buf[hp + 2];
            }
            tg_h1[c][sy][sx] = v1;
            tg_h2[c][sy][sx] = v2;
            tg_h3[c][sy][sx] = v3;
        }
    }
}
