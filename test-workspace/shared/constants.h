#pragma once

constant float PI = 3.14159265358979323846f;
constant float TAU = 2.0f * PI;
constant float INV_PI = 1.0f / PI;
constant uint TILE_SIZE = 16;
constant uint MAX_LIGHTS = 128;

struct Light {
    float3 position;
    float3 color;
    float intensity;
    float radius;
};
