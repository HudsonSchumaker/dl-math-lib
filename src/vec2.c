/**
 * vec2.c - Additional optimized 2D vector operations
 */

#include "../include/mathlib.h"

vec2_t vec2_perpendicular(vec2_t v) {
    return (vec2_t){{.x = -v.y, .y = v.x}};
}

float vec2_cross(vec2_t a, vec2_t b) {
    return a.x * b.y - a.y * b.x;
}

float vec2_angle_between(vec2_t a, vec2_t b) {
    float dot = vec2_dot(a, b);
    float len_a = vec2_length(a);
    float len_b = vec2_length(b);

    if (len_a < ML_EPSILON || len_b < ML_EPSILON) {
        return 0.0f;
    }

    return acosf(ml_clamp(dot / (len_a * len_b), -1.0f, 1.0f));
}

vec2_t vec2_project_onto(vec2_t v, vec2_t onto) {
    float dot = vec2_dot(v, onto);
    float len_sq = vec2_length_sq(onto);

    if (len_sq < ML_EPSILON) {
        return (vec2_t){{.x = 0, .y = 0}};
    }

    return vec2_mul(onto, dot / len_sq);
}

vec2_t vec2_min(vec2_t a, vec2_t b) {
    return (vec2_t){{
        .x = fminf(a.x, b.x),
        .y = fminf(a.y, b.y)
    }};
}

vec2_t vec2_max(vec2_t a, vec2_t b) {
    return (vec2_t){{
        .x = fmaxf(a.x, b.x),
        .y = fmaxf(a.y, b.y)
    }};
}

vec2_t vec2_clamp(vec2_t v, vec2_t min, vec2_t max) {
    return (vec2_t){{
        .x = ml_clamp(v.x, min.x, max.x),
        .y = ml_clamp(v.y, min.y, max.y)
    }};
}
