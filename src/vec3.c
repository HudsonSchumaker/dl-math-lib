/**
 * vec3.c - Additional optimized 3D vector operations
 */

#include "../include/mathlib.h"

float vec3_angle_between(vec3_t a, vec3_t b) {
    float dot = vec3_dot(a, b);
    float len_a = vec3_length(a);
    float len_b = vec3_length(b);

    if (len_a < ML_EPSILON || len_b < ML_EPSILON) {
        return 0.0f;
    }

    return acosf(ml_clamp(dot / (len_a * len_b), -1.0f, 1.0f));
}

vec3_t vec3_min(vec3_t a, vec3_t b) {
    return (vec3_t){{
        .x = fminf(a.x, b.x),
        .y = fminf(a.y, b.y),
        .z = fminf(a.z, b.z),
        ._pad = 0
    }};
}

vec3_t vec3_max(vec3_t a, vec3_t b) {
    return (vec3_t){{
        .x = fmaxf(a.x, b.x),
        .y = fmaxf(a.y, b.y),
        .z = fmaxf(a.z, b.z),
        ._pad = 0
    }};
}

vec3_t vec3_clamp(vec3_t v, vec3_t min, vec3_t max) {
    return (vec3_t){{
        .x = ml_clamp(v.x, min.x, max.x),
        .y = ml_clamp(v.y, min.y, max.y),
        .z = ml_clamp(v.z, min.z, max.z),
        ._pad = 0
    }};
}

vec3_t vec3_rotate_x(vec3_t v, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    return (vec3_t){{
        .x = v.x,
        .y = v.y * c - v.z * s,
        .z = v.y * s + v.z * c,
        ._pad = 0
    }};
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    return (vec3_t){{
        .x = v.x * c + v.z * s,
        .y = v.y,
        .z = -v.x * s + v.z * c,
        ._pad = 0
    }};
}

vec3_t vec3_rotate_z(vec3_t v, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    return (vec3_t){{
        .x = v.x * c - v.y * s,
        .y = v.x * s + v.y * c,
        .z = v.z,
        ._pad = 0
    }};
}

vec3_t vec3_rotate_axis(vec3_t v, vec3_t axis, float angle) {
    axis = vec3_normalize(axis);
    float c = cosf(angle);
    float s = sinf(angle);
    float one_minus_c = 1.0f - c;

    float x = axis.x, y = axis.y, z = axis.z;

    return (vec3_t){{
        .x = (x * x * one_minus_c + c) * v.x +
             (x * y * one_minus_c - z * s) * v.y +
             (x * z * one_minus_c + y * s) * v.z,
        .y = (y * x * one_minus_c + z * s) * v.x +
             (y * y * one_minus_c + c) * v.y +
             (y * z * one_minus_c - x * s) * v.z,
        .z = (z * x * one_minus_c - y * s) * v.x +
             (z * y * one_minus_c + x * s) * v.y +
             (z * z * one_minus_c + c) * v.z,
        ._pad = 0
    }};
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, float t) {
    float dot = ml_clamp(vec3_dot(a, b), -1.0f, 1.0f);
    float theta = acosf(dot) * t;

    vec3_t relative = vec3_normalize(vec3_sub(b, vec3_mul(a, dot)));

    return vec3_add(
        vec3_mul(a, cosf(theta)),
        vec3_mul(relative, sinf(theta))
    );
}

bool vec3_nearly_equal(vec3_t a, vec3_t b, float epsilon) {
    return fabsf(a.x - b.x) < epsilon &&
           fabsf(a.y - b.y) < epsilon &&
           fabsf(a.z - b.z) < epsilon;
}
