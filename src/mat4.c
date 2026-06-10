/**
 * mat4.c - 4x4 matrix operations (3D transformations)
 * Column-major layout for OpenGL compatibility
 */

#include "../include/mathlib.h"
#include <string.h>

mat4_t mat4_identity(void) {
    return (mat4_t){{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }};
}

mat4_t mat4_mul(const mat4_t *a, const mat4_t *b) {
    mat4_t result;

#ifdef MATHLIB_SSE
    for (int i = 0; i < 4; i++) {
        __m128 col = b->cols[i].simd;
        __m128 r0 = _mm_mul_ps(_mm_shuffle_ps(col, col, 0x00), a->cols[0].simd);
        __m128 r1 = _mm_mul_ps(_mm_shuffle_ps(col, col, 0x55), a->cols[1].simd);
        __m128 r2 = _mm_mul_ps(_mm_shuffle_ps(col, col, 0xAA), a->cols[2].simd);
        __m128 r3 = _mm_mul_ps(_mm_shuffle_ps(col, col, 0xFF), a->cols[3].simd);
        result.cols[i].simd = _mm_add_ps(_mm_add_ps(r0, r1), _mm_add_ps(r2, r3));
    }
#else
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            result.m[col * 4 + row] =
                a->m[0 * 4 + row] * b->m[col * 4 + 0] +
                a->m[1 * 4 + row] * b->m[col * 4 + 1] +
                a->m[2 * 4 + row] * b->m[col * 4 + 2] +
                a->m[3 * 4 + row] * b->m[col * 4 + 3];
        }
    }
#endif

    return result;
}

vec4_t mat4_mul_vec4(const mat4_t *m, vec4_t v) {
#ifdef MATHLIB_SSE
    __m128 x = _mm_shuffle_ps(v.simd, v.simd, 0x00);
    __m128 y = _mm_shuffle_ps(v.simd, v.simd, 0x55);
    __m128 z = _mm_shuffle_ps(v.simd, v.simd, 0xAA);
    __m128 w = _mm_shuffle_ps(v.simd, v.simd, 0xFF);

    __m128 r0 = _mm_mul_ps(m->cols[0].simd, x);
    __m128 r1 = _mm_mul_ps(m->cols[1].simd, y);
    __m128 r2 = _mm_mul_ps(m->cols[2].simd, z);
    __m128 r3 = _mm_mul_ps(m->cols[3].simd, w);

    vec4_t result;
    result.simd = _mm_add_ps(_mm_add_ps(r0, r1), _mm_add_ps(r2, r3));
    return result;
#else
    return (vec4_t){{
        .x = m->m[0] * v.x + m->m[4] * v.y + m->m[8]  * v.z + m->m[12] * v.w,
        .y = m->m[1] * v.x + m->m[5] * v.y + m->m[9]  * v.z + m->m[13] * v.w,
        .z = m->m[2] * v.x + m->m[6] * v.y + m->m[10] * v.z + m->m[14] * v.w,
        .w = m->m[3] * v.x + m->m[7] * v.y + m->m[11] * v.z + m->m[15] * v.w
    }};
#endif
}

vec3_t mat4_mul_vec3(const mat4_t *m, vec3_t v, float w) {
    vec4_t v4 = vec4_from_vec3(v, w);
    vec4_t result = mat4_mul_vec4(m, v4);

    if (fabsf(result.w) > ML_EPSILON) {
        float inv_w = 1.0f / result.w;
        return vec3_new(result.x * inv_w, result.y * inv_w, result.z * inv_w);
    }
    return vec3_new(result.x, result.y, result.z);
}

mat4_t mat4_translate(float x, float y, float z) {
    return (mat4_t){{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    }};
}

mat4_t mat4_rotate_x(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    return (mat4_t){{
        1, 0,  0, 0,
        0, c,  s, 0,
        0, -s, c, 0,
        0, 0,  0, 1
    }};
}

mat4_t mat4_rotate_y(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    return (mat4_t){{
        c, 0, -s, 0,
        0, 1,  0, 0,
        s, 0,  c, 0,
        0, 0,  0, 1
    }};
}

mat4_t mat4_rotate_z(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    return (mat4_t){{
        c,  s, 0, 0,
        -s, c, 0, 0,
        0,  0, 1, 0,
        0,  0, 0, 1
    }};
}

mat4_t mat4_rotate(float angle, vec3_t axis) {
    axis = vec3_normalize(axis);
    float c = cosf(angle);
    float s = sinf(angle);
    float t = 1.0f - c;

    float x = axis.x, y = axis.y, z = axis.z;

    return (mat4_t){{
        t*x*x + c,    t*x*y + s*z,  t*x*z - s*y,  0,
        t*x*y - s*z,  t*y*y + c,    t*y*z + s*x,  0,
        t*x*z + s*y,  t*y*z - s*x,  t*z*z + c,    0,
        0,            0,            0,            1
    }};
}

mat4_t mat4_scale(float sx, float sy, float sz) {
    return (mat4_t){{
        sx, 0,  0,  0,
        0,  sy, 0,  0,
        0,  0,  sz, 0,
        0,  0,  0,  1
    }};
}

mat4_t mat4_transpose(const mat4_t *m) {
#ifdef MATHLIB_SSE
    mat4_t result;
    __m128 tmp0 = _mm_unpacklo_ps(m->cols[0].simd, m->cols[1].simd);
    __m128 tmp1 = _mm_unpacklo_ps(m->cols[2].simd, m->cols[3].simd);
    __m128 tmp2 = _mm_unpackhi_ps(m->cols[0].simd, m->cols[1].simd);
    __m128 tmp3 = _mm_unpackhi_ps(m->cols[2].simd, m->cols[3].simd);
    result.cols[0].simd = _mm_movelh_ps(tmp0, tmp1);
    result.cols[1].simd = _mm_movehl_ps(tmp1, tmp0);
    result.cols[2].simd = _mm_movelh_ps(tmp2, tmp3);
    result.cols[3].simd = _mm_movehl_ps(tmp3, tmp2);
    return result;
#else
    return (mat4_t){{
        m->m[0], m->m[4], m->m[8],  m->m[12],
        m->m[1], m->m[5], m->m[9],  m->m[13],
        m->m[2], m->m[6], m->m[10], m->m[14],
        m->m[3], m->m[7], m->m[11], m->m[15]
    }};
#endif
}

float mat4_determinant(const mat4_t *m) {
    float a0 = m->m[0]  * m->m[5]  - m->m[1]  * m->m[4];
    float a1 = m->m[0]  * m->m[6]  - m->m[2]  * m->m[4];
    float a2 = m->m[0]  * m->m[7]  - m->m[3]  * m->m[4];
    float a3 = m->m[1]  * m->m[6]  - m->m[2]  * m->m[5];
    float a4 = m->m[1]  * m->m[7]  - m->m[3]  * m->m[5];
    float a5 = m->m[2]  * m->m[7]  - m->m[3]  * m->m[6];
    float b0 = m->m[8]  * m->m[13] - m->m[9]  * m->m[12];
    float b1 = m->m[8]  * m->m[14] - m->m[10] * m->m[12];
    float b2 = m->m[8]  * m->m[15] - m->m[11] * m->m[12];
    float b3 = m->m[9]  * m->m[14] - m->m[10] * m->m[13];
    float b4 = m->m[9]  * m->m[15] - m->m[11] * m->m[13];
    float b5 = m->m[10] * m->m[15] - m->m[11] * m->m[14];

    return a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
}

mat4_t mat4_inverse(const mat4_t *m) {
    float a0 = m->m[0]  * m->m[5]  - m->m[1]  * m->m[4];
    float a1 = m->m[0]  * m->m[6]  - m->m[2]  * m->m[4];
    float a2 = m->m[0]  * m->m[7]  - m->m[3]  * m->m[4];
    float a3 = m->m[1]  * m->m[6]  - m->m[2]  * m->m[5];
    float a4 = m->m[1]  * m->m[7]  - m->m[3]  * m->m[5];
    float a5 = m->m[2]  * m->m[7]  - m->m[3]  * m->m[6];
    float b0 = m->m[8]  * m->m[13] - m->m[9]  * m->m[12];
    float b1 = m->m[8]  * m->m[14] - m->m[10] * m->m[12];
    float b2 = m->m[8]  * m->m[15] - m->m[11] * m->m[12];
    float b3 = m->m[9]  * m->m[14] - m->m[10] * m->m[13];
    float b4 = m->m[9]  * m->m[15] - m->m[11] * m->m[13];
    float b5 = m->m[10] * m->m[15] - m->m[11] * m->m[14];

    float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
    if (fabsf(det) < ML_EPSILON) {
        return mat4_identity();
    }

    mat4_t result;
    float inv_det = 1.0f / det;

    result.m[0]  = ( m->m[5]  * b5 - m->m[6]  * b4 + m->m[7]  * b3) * inv_det;
    result.m[1]  = (-m->m[1]  * b5 + m->m[2]  * b4 - m->m[3]  * b3) * inv_det;
    result.m[2]  = ( m->m[13] * a5 - m->m[14] * a4 + m->m[15] * a3) * inv_det;
    result.m[3]  = (-m->m[9]  * a5 + m->m[10] * a4 - m->m[11] * a3) * inv_det;
    result.m[4]  = (-m->m[4]  * b5 + m->m[6]  * b2 - m->m[7]  * b1) * inv_det;
    result.m[5]  = ( m->m[0]  * b5 - m->m[2]  * b2 + m->m[3]  * b1) * inv_det;
    result.m[6]  = (-m->m[12] * a5 + m->m[14] * a2 - m->m[15] * a1) * inv_det;
    result.m[7]  = ( m->m[8]  * a5 - m->m[10] * a2 + m->m[11] * a1) * inv_det;
    result.m[8]  = ( m->m[4]  * b4 - m->m[5]  * b2 + m->m[7]  * b0) * inv_det;
    result.m[9]  = (-m->m[0]  * b4 + m->m[1]  * b2 - m->m[3]  * b0) * inv_det;
    result.m[10] = ( m->m[12] * a4 - m->m[13] * a2 + m->m[15] * a0) * inv_det;
    result.m[11] = (-m->m[8]  * a4 + m->m[9]  * a2 - m->m[11] * a0) * inv_det;
    result.m[12] = (-m->m[4]  * b3 + m->m[5]  * b1 - m->m[6]  * b0) * inv_det;
    result.m[13] = ( m->m[0]  * b3 - m->m[1]  * b1 + m->m[2]  * b0) * inv_det;
    result.m[14] = (-m->m[12] * a3 + m->m[13] * a1 - m->m[14] * a0) * inv_det;
    result.m[15] = ( m->m[8]  * a3 - m->m[9]  * a1 + m->m[10] * a0) * inv_det;

    return result;
}

mat4_t mat4_look_at(vec3_t eye, vec3_t center, vec3_t up) {
    vec3_t f = vec3_normalize(vec3_sub(center, eye));
    vec3_t s = vec3_normalize(vec3_cross(f, up));
    vec3_t u = vec3_cross(s, f);

    mat4_t result = mat4_identity();
    result.m[0] = s.x;
    result.m[4] = s.y;
    result.m[8] = s.z;
    result.m[1] = u.x;
    result.m[5] = u.y;
    result.m[9] = u.z;
    result.m[2] = -f.x;
    result.m[6] = -f.y;
    result.m[10] = -f.z;
    result.m[12] = -vec3_dot(s, eye);
    result.m[13] = -vec3_dot(u, eye);
    result.m[14] = vec3_dot(f, eye);

    return result;
}

mat4_t mat4_perspective(float fov_y, float aspect, float near, float far) {
    float tan_half_fov = tanf(fov_y * 0.5f);
    float range = near - far;

    mat4_t result = {0};
    result.m[0] = 1.0f / (aspect * tan_half_fov);
    result.m[5] = 1.0f / tan_half_fov;
    result.m[10] = (near + far) / range;
    result.m[11] = -1.0f;
    result.m[14] = (2.0f * near * far) / range;

    return result;
}

mat4_t mat4_orthographic(float left, float right, float bottom, float top, float near, float far) {
    mat4_t result = mat4_identity();
    result.m[0] = 2.0f / (right - left);
    result.m[5] = 2.0f / (top - bottom);
    result.m[10] = -2.0f / (far - near);
    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    result.m[14] = -(far + near) / (far - near);

    return result;
}

mat4_t mat4_isometric(float width, float height, float near, float far) {
    float half_width = width * 0.5f;
    float half_height = height * 0.5f;
    return mat4_orthographic(-half_width, half_width, -half_height, half_height, near, far);
}

mat4_t mat4_isometric_view(vec3_t target, float distance) {
    // Classic isometric angles:
    // Rotate 45° around Y (horizontal rotation)
    // Then rotate ~35.264° around X (pitch angle for isometric view)
    // This gives the classic isometric look where all three axes are equally foreshortened

    float angle_y = ML_PI * 0.25f;  // 45 degrees
    float angle_x = atanf(1.0f / sqrtf(2.0f));  // ~35.264 degrees (arctan(1/√2))

    // Calculate camera position based on target and distance
    // Position camera at the isometric angle relative to target
    float cos_y = cosf(angle_y);
    float sin_y = sinf(angle_y);
    float cos_x = cosf(angle_x);
    float sin_x = sinf(angle_x);

    vec3_t offset = vec3_new(
        distance * cos_x * sin_y,
        distance * sin_x,
        distance * cos_x * cos_y
    );

    vec3_t eye = vec3_add(target, offset);
    vec3_t up = vec3_new(0, 1, 0);

    return mat4_look_at(eye, target, up);
}
