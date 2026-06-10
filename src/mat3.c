/**
 * mat3.c - 3x3 matrix operations (2D transformations)
 */

#include "../include/mathlib.h"
#include <string.h>

mat3_t mat3_identity(void) {
    return (mat3_t){{
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    }};
}

mat3_t mat3_mul(const mat3_t *a, const mat3_t *b) {
    mat3_t result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i * 3 + j] =
                a->m[i * 3 + 0] * b->m[0 * 3 + j] +
                a->m[i * 3 + 1] * b->m[1 * 3 + j] +
                a->m[i * 3 + 2] * b->m[2 * 3 + j];
        }
    }
    return result;
}

vec2_t mat3_mul_vec2(const mat3_t *m, vec2_t v) {
    float x = m->m[0] * v.x + m->m[1] * v.y + m->m[2];
    float y = m->m[3] * v.x + m->m[4] * v.y + m->m[5];
    float w = m->m[6] * v.x + m->m[7] * v.y + m->m[8];

    if (fabsf(w) > ML_EPSILON) {
        return vec2_new(x / w, y / w);
    }
    return vec2_new(x, y);
}

mat3_t mat3_translate(float x, float y) {
    return (mat3_t){{
        1, 0, x,
        0, 1, y,
        0, 0, 1
    }};
}

mat3_t mat3_rotate(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    return (mat3_t){{
        c, -s, 0,
        s,  c, 0,
        0,  0, 1
    }};
}

mat3_t mat3_scale(float sx, float sy) {
    return (mat3_t){{
        sx, 0,  0,
        0,  sy, 0,
        0,  0,  1
    }};
}

float mat3_determinant(const mat3_t *m) {
    return m->m[0] * (m->m[4] * m->m[8] - m->m[5] * m->m[7]) -
           m->m[1] * (m->m[3] * m->m[8] - m->m[5] * m->m[6]) +
           m->m[2] * (m->m[3] * m->m[7] - m->m[4] * m->m[6]);
}

mat3_t mat3_inverse(const mat3_t *m) {
    float det = mat3_determinant(m);
    if (fabsf(det) < ML_EPSILON) {
        return mat3_identity();
    }

    float inv_det = 1.0f / det;
    mat3_t result;

    result.m[0] = (m->m[4] * m->m[8] - m->m[5] * m->m[7]) * inv_det;
    result.m[1] = (m->m[2] * m->m[7] - m->m[1] * m->m[8]) * inv_det;
    result.m[2] = (m->m[1] * m->m[5] - m->m[2] * m->m[4]) * inv_det;
    result.m[3] = (m->m[5] * m->m[6] - m->m[3] * m->m[8]) * inv_det;
    result.m[4] = (m->m[0] * m->m[8] - m->m[2] * m->m[6]) * inv_det;
    result.m[5] = (m->m[2] * m->m[3] - m->m[0] * m->m[5]) * inv_det;
    result.m[6] = (m->m[3] * m->m[7] - m->m[4] * m->m[6]) * inv_det;
    result.m[7] = (m->m[1] * m->m[6] - m->m[0] * m->m[7]) * inv_det;
    result.m[8] = (m->m[0] * m->m[4] - m->m[1] * m->m[3]) * inv_det;

    return result;
}
