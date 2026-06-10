/**
 * quat.c - Quaternion operations for smooth rotations
 */

#include "../include/mathlib.h"

quat_t quat_from_axis_angle(vec3_t axis, float angle) {
    axis = vec3_normalize(axis);
    float half_angle = angle * 0.5f;
    float s = sinf(half_angle);
    float c = cosf(half_angle);

    return (quat_t){{
        .x = axis.x * s,
        .y = axis.y * s,
        .z = axis.z * s,
        .w = c
    }};
}

quat_t quat_from_euler(float pitch, float yaw, float roll) {
    float cp = cosf(pitch * 0.5f);
    float sp = sinf(pitch * 0.5f);
    float cy = cosf(yaw * 0.5f);
    float sy = sinf(yaw * 0.5f);
    float cr = cosf(roll * 0.5f);
    float sr = sinf(roll * 0.5f);

    return (quat_t){{
        .w = cr * cp * cy + sr * sp * sy,
        .x = sr * cp * cy - cr * sp * sy,
        .y = cr * sp * cy + sr * cp * sy,
        .z = cr * cp * sy - sr * sp * cy
    }};
}

quat_t quat_mul(quat_t a, quat_t b) {
    return (quat_t){{
        .w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
        .x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        .y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        .z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w
    }};
}

quat_t quat_conjugate(quat_t q) {
    return (quat_t){{.x = -q.x, .y = -q.y, .z = -q.z, .w = q.w}};
}

float quat_dot(quat_t a, quat_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

quat_t quat_normalize(quat_t q) {
    float len = sqrtf(quat_dot(q, q));
    if (len < ML_EPSILON) {
        return quat_identity();
    }

    float inv_len = 1.0f / len;
    return (quat_t){{
        .x = q.x * inv_len,
        .y = q.y * inv_len,
        .z = q.z * inv_len,
        .w = q.w * inv_len
    }};
}

quat_t quat_inverse(quat_t q) {
    float dot = quat_dot(q, q);
    if (dot < ML_EPSILON) {
        return quat_identity();
    }

    quat_t conj = quat_conjugate(q);
    float inv_dot = 1.0f / dot;
    return (quat_t){{
        .x = conj.x * inv_dot,
        .y = conj.y * inv_dot,
        .z = conj.z * inv_dot,
        .w = conj.w * inv_dot
    }};
}

quat_t quat_slerp(quat_t a, quat_t b, float t) {
    float dot = quat_dot(a, b);

    if (dot < 0.0f) {
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        b.w = -b.w;
        dot = -dot;
    }

    if (dot > 0.9995f) {
        return quat_normalize((quat_t){{
            .x = a.x + t * (b.x - a.x),
            .y = a.y + t * (b.y - a.y),
            .z = a.z + t * (b.z - a.z),
            .w = a.w + t * (b.w - a.w)
        }});
    }

    float theta_0 = acosf(dot);
    float theta = theta_0 * t;
    float sin_theta = sinf(theta);
    float sin_theta_0 = sinf(theta_0);

    float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;

    return (quat_t){{
        .x = s0 * a.x + s1 * b.x,
        .y = s0 * a.y + s1 * b.y,
        .z = s0 * a.z + s1 * b.z,
        .w = s0 * a.w + s1 * b.w
    }};
}

vec3_t quat_rotate_vec3(quat_t q, vec3_t v) {
    vec3_t qv = vec3_new(q.x, q.y, q.z);
    vec3_t uv = vec3_cross(qv, v);
    vec3_t uuv = vec3_cross(qv, uv);

    uv = vec3_mul(uv, 2.0f * q.w);
    uuv = vec3_mul(uuv, 2.0f);

    return vec3_add(vec3_add(v, uv), uuv);
}

mat4_t quat_to_mat4(quat_t q) {
    q = quat_normalize(q);

    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    return (mat4_t){{
        1 - 2*(yy + zz), 2*(xy + wz),     2*(xz - wy),     0,
        2*(xy - wz),     1 - 2*(xx + zz), 2*(yz + wx),     0,
        2*(xz + wy),     2*(yz - wx),     1 - 2*(xx + yy), 0,
        0,               0,               0,               1
    }};
}
