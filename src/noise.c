/**
 * noise.c - Noise generation functions for procedural content
 */

#include "../include/mathlib.h"

// Hash function for noise generation
static inline uint32_t hash_uint32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

// 1D value noise
float noise_value_1d(float x) {
    int32_t i = (int32_t)floorf(x);
    float f = x - i;

    uint32_t h0 = hash_uint32((uint32_t)i);
    uint32_t h1 = hash_uint32((uint32_t)(i + 1));

    float v0 = (float)(h0 & 0xFFFFFF) / (float)0xFFFFFF;
    float v1 = (float)(h1 & 0xFFFFFF) / (float)0xFFFFFF;

    float t = ml_smoothstep(0, 1, f);
    return ml_lerp(v0, v1, t);
}

// 2D value noise
float noise_value_2d(float x, float y) {
    int32_t ix = (int32_t)floorf(x);
    int32_t iy = (int32_t)floorf(y);
    float fx = x - ix;
    float fy = y - iy;

    uint32_t h00 = hash_uint32(hash_uint32((uint32_t)ix) + (uint32_t)iy);
    uint32_t h10 = hash_uint32(hash_uint32((uint32_t)(ix + 1)) + (uint32_t)iy);
    uint32_t h01 = hash_uint32(hash_uint32((uint32_t)ix) + (uint32_t)(iy + 1));
    uint32_t h11 = hash_uint32(hash_uint32((uint32_t)(ix + 1)) + (uint32_t)(iy + 1));

    float v00 = (float)(h00 & 0xFFFFFF) / (float)0xFFFFFF;
    float v10 = (float)(h10 & 0xFFFFFF) / (float)0xFFFFFF;
    float v01 = (float)(h01 & 0xFFFFFF) / (float)0xFFFFFF;
    float v11 = (float)(h11 & 0xFFFFFF) / (float)0xFFFFFF;

    float tx = ml_smoothstep(0, 1, fx);
    float ty = ml_smoothstep(0, 1, fy);

    float a = ml_lerp(v00, v10, tx);
    float b = ml_lerp(v01, v11, tx);

    return ml_lerp(a, b, ty);
}

// 3D value noise
float noise_value_3d(float x, float y, float z) {
    int32_t ix = (int32_t)floorf(x);
    int32_t iy = (int32_t)floorf(y);
    int32_t iz = (int32_t)floorf(z);
    float fx = x - ix;
    float fy = y - iy;
    float fz = z - iz;

    uint32_t base = hash_uint32((uint32_t)ix);
    base = hash_uint32(base + (uint32_t)iy);

    uint32_t h000 = hash_uint32(base + (uint32_t)iz);
    uint32_t h100 = hash_uint32(hash_uint32((uint32_t)(ix + 1)) + (uint32_t)iy);
    h100 = hash_uint32(h100 + (uint32_t)iz);

    uint32_t h010 = hash_uint32(base + (uint32_t)(iz));
    uint32_t h110 = hash_uint32(hash_uint32((uint32_t)(ix + 1)) + (uint32_t)(iy + 1));
    h110 = hash_uint32(h110 + (uint32_t)iz);

    uint32_t h001 = hash_uint32(base + (uint32_t)(iz + 1));
    uint32_t h101 = hash_uint32(hash_uint32((uint32_t)(ix + 1)) + (uint32_t)iy);
    h101 = hash_uint32(h101 + (uint32_t)(iz + 1));

    uint32_t h011 = hash_uint32(base + (uint32_t)(iz + 1));
    uint32_t h111 = hash_uint32(hash_uint32((uint32_t)(ix + 1)) + (uint32_t)(iy + 1));
    h111 = hash_uint32(h111 + (uint32_t)(iz + 1));

    float v000 = (float)(h000 & 0xFFFFFF) / (float)0xFFFFFF;
    float v100 = (float)(h100 & 0xFFFFFF) / (float)0xFFFFFF;
    float v010 = (float)(h010 & 0xFFFFFF) / (float)0xFFFFFF;
    float v110 = (float)(h110 & 0xFFFFFF) / (float)0xFFFFFF;
    float v001 = (float)(h001 & 0xFFFFFF) / (float)0xFFFFFF;
    float v101 = (float)(h101 & 0xFFFFFF) / (float)0xFFFFFF;
    float v011 = (float)(h011 & 0xFFFFFF) / (float)0xFFFFFF;
    float v111 = (float)(h111 & 0xFFFFFF) / (float)0xFFFFFF;

    float tx = ml_smoothstep(0, 1, fx);
    float ty = ml_smoothstep(0, 1, fy);
    float tz = ml_smoothstep(0, 1, fz);

    float a = ml_lerp(v000, v100, tx);
    float b = ml_lerp(v010, v110, tx);
    float c = ml_lerp(v001, v101, tx);
    float d = ml_lerp(v011, v111, tx);

    float e = ml_lerp(a, b, ty);
    float f = ml_lerp(c, d, ty);

    return ml_lerp(e, f, tz);
}

// Fractal Brownian Motion (fBm)
float noise_fbm_2d(float x, float y, int octaves, float lacunarity, float gain) {
    float sum = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float max_value = 0.0f;

    for (int i = 0; i < octaves; i++) {
        sum += noise_value_2d(x * frequency, y * frequency) * amplitude;
        max_value += amplitude;
        amplitude *= gain;
        frequency *= lacunarity;
    }

    return sum / max_value;
}

float noise_fbm_3d(float x, float y, float z, int octaves, float lacunarity, float gain) {
    float sum = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float max_value = 0.0f;

    for (int i = 0; i < octaves; i++) {
        sum += noise_value_3d(x * frequency, y * frequency, z * frequency) * amplitude;
        max_value += amplitude;
        amplitude *= gain;
        frequency *= lacunarity;
    }

    return sum / max_value;
}

// Turbulence (absolute value of fBm)
float noise_turbulence_2d(float x, float y, int octaves) {
    float sum = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;

    for (int i = 0; i < octaves; i++) {
        sum += fabsf(noise_value_2d(x * frequency, y * frequency) * 2.0f - 1.0f) * amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }

    return sum;
}

// Ridged noise (inverted turbulence)
float noise_ridged_2d(float x, float y, int octaves) {
    float sum = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;

    for (int i = 0; i < octaves; i++) {
        float n = 1.0f - fabsf(noise_value_2d(x * frequency, y * frequency) * 2.0f - 1.0f);
        sum += n * n * amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }

    return sum;
}

// Cellular/Worley noise (simplified version)
float noise_cellular_2d(float x, float y) {
    int ix = (int)floorf(x);
    int iy = (int)floorf(y);

    float min_dist = 1e10f;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int cell_x = ix + dx;
            int cell_y = iy + dy;

            uint32_t h = hash_uint32(hash_uint32((uint32_t)cell_x) + (uint32_t)cell_y);
            float point_x = cell_x + (float)(h & 0xFFFF) / (float)0xFFFF;
            float point_y = cell_y + (float)((h >> 16) & 0xFFFF) / (float)0xFFFF;

            float dx_sq = (x - point_x) * (x - point_x);
            float dy_sq = (y - point_y) * (y - point_y);
            float dist = sqrtf(dx_sq + dy_sq);

            if (dist < min_dist) {
                min_dist = dist;
            }
        }
    }

    return min_dist;
}
