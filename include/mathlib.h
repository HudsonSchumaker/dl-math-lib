/**
 * mathlib.h - High-performance 2D/3D graphics math library (C17)
 *
 * Features:
 * - SIMD-optimized operations (SSE/AVX on x86, NEON on ARM)
 * - Cache-friendly memory layout
 * - Inline functions for minimal overhead
 * - Common graphics operations (matrices, vectors, quaternions)
 */

#ifndef MATHLIB_H
#define MATHLIB_H

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Platform detection and SIMD support
// ============================================================================

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
    #define MATHLIB_X86
    #if defined(__AVX__)
        #define MATHLIB_AVX
        #include <immintrin.h>
    #elif defined(__SSE__)
        #define MATHLIB_SSE
        #include <xmmintrin.h>
        #include <emmintrin.h>
    #endif
#elif defined(__ARM_NEON) || defined(__aarch64__)
    #define MATHLIB_NEON
    #include <arm_neon.h>
#endif

// ============================================================================
// Constants
// ============================================================================

#define ML_PI          3.14159265358979323846
#define ML_PI_2        1.57079632679489661923
#define ML_PI_4        0.78539816339744830962
#define ML_TAU         6.28318530717958647692
#define ML_E           2.71828182845904523536
#define ML_SQRT2       1.41421356237309504880
#define ML_EPSILON     1e-6f
#define ML_DEG2RAD     0.01745329251994329576
#define ML_RAD2DEG     57.2957795130823208767

// ============================================================================
// 2D Vector (aligned for SIMD)
// ============================================================================

typedef struct {
    union {
        struct { float x, y; };
        float data[2];
    };
} __attribute__((aligned(8))) vec2_t;

// 2D Vector operations
static inline vec2_t vec2_new(float x, float y) {
    return (vec2_t){{.x = x, .y = y}};
}

static inline vec2_t vec2_add(vec2_t a, vec2_t b) {
    return (vec2_t){{.x = a.x + b.x, .y = a.y + b.y}};
}

static inline vec2_t vec2_sub(vec2_t a, vec2_t b) {
    return (vec2_t){{.x = a.x - b.x, .y = a.y - b.y}};
}

static inline vec2_t vec2_mul(vec2_t v, float s) {
    return (vec2_t){{.x = v.x * s, .y = v.y * s}};
}

static inline vec2_t vec2_div(vec2_t v, float s) {
    float inv = 1.0f / s;
    return (vec2_t){{.x = v.x * inv, .y = v.y * inv}};
}

static inline float vec2_dot(vec2_t a, vec2_t b) {
    return a.x * b.x + a.y * b.y;
}

static inline float vec2_length_sq(vec2_t v) {
    return v.x * v.x + v.y * v.y;
}

static inline float vec2_length(vec2_t v) {
    return sqrtf(vec2_length_sq(v));
}

static inline vec2_t vec2_normalize(vec2_t v) {
    float len = vec2_length(v);
    return len > ML_EPSILON ? vec2_div(v, len) : (vec2_t){{.x = 0, .y = 0}};
}

static inline vec2_t vec2_lerp(vec2_t a, vec2_t b, float t) {
    return (vec2_t){{
        .x = a.x + (b.x - a.x) * t,
        .y = a.y + (b.y - a.y) * t
    }};
}

static inline float vec2_distance(vec2_t a, vec2_t b) {
    return vec2_length(vec2_sub(b, a));
}

static inline vec2_t vec2_rotate(vec2_t v, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    return (vec2_t){{
        .x = v.x * c - v.y * s,
        .y = v.x * s + v.y * c
    }};
}

static inline vec2_t vec2_reflect(vec2_t v, vec2_t n) {
    float dot = vec2_dot(v, n);
    return vec2_sub(v, vec2_mul(n, 2.0f * dot));
}

vec2_t vec2_perpendicular(vec2_t v);
float vec2_cross(vec2_t a, vec2_t b);
float vec2_angle_between(vec2_t a, vec2_t b);
vec2_t vec2_project_onto(vec2_t v, vec2_t onto);
vec2_t vec2_min(vec2_t a, vec2_t b);
vec2_t vec2_max(vec2_t a, vec2_t b);
vec2_t vec2_clamp(vec2_t v, vec2_t min, vec2_t max);

// ============================================================================
// 3D Vector (aligned for SIMD)
// ============================================================================

typedef struct {
    union {
        struct { float x, y, z, _pad; };
        float data[4];
#ifdef MATHLIB_SSE
        __m128 simd;
#endif
#ifdef MATHLIB_NEON
        float32x4_t simd;
#endif
    };
} __attribute__((aligned(16))) vec3_t;

// 3D Vector operations
static inline vec3_t vec3_new(float x, float y, float z) {
    return (vec3_t){{.x = x, .y = y, .z = z, ._pad = 0.0f}};
}

#ifdef MATHLIB_SSE
static inline vec3_t vec3_add(vec3_t a, vec3_t b) {
    vec3_t result;
    result.simd = _mm_add_ps(a.simd, b.simd);
    return result;
}

static inline vec3_t vec3_sub(vec3_t a, vec3_t b) {
    vec3_t result;
    result.simd = _mm_sub_ps(a.simd, b.simd);
    return result;
}

static inline vec3_t vec3_mul(vec3_t v, float s) {
    vec3_t result;
    __m128 scalar = _mm_set1_ps(s);
    result.simd = _mm_mul_ps(v.simd, scalar);
    return result;
}
#elif defined(MATHLIB_NEON)
static inline vec3_t vec3_add(vec3_t a, vec3_t b) {
    vec3_t result;
    result.simd = vaddq_f32(a.simd, b.simd);
    return result;
}

static inline vec3_t vec3_sub(vec3_t a, vec3_t b) {
    vec3_t result;
    result.simd = vsubq_f32(a.simd, b.simd);
    return result;
}

static inline vec3_t vec3_mul(vec3_t v, float s) {
    vec3_t result;
    float32x4_t scalar = vdupq_n_f32(s);
    result.simd = vmulq_f32(v.simd, scalar);
    return result;
}
#else
static inline vec3_t vec3_add(vec3_t a, vec3_t b) {
    return (vec3_t){{.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z}};
}

static inline vec3_t vec3_sub(vec3_t a, vec3_t b) {
    return (vec3_t){{.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z}};
}

static inline vec3_t vec3_mul(vec3_t v, float s) {
    return (vec3_t){{.x = v.x * s, .y = v.y * s, .z = v.z * s}};
}
#endif

static inline vec3_t vec3_div(vec3_t v, float s) {
    return vec3_mul(v, 1.0f / s);
}

static inline float vec3_dot(vec3_t a, vec3_t b) {
#ifdef MATHLIB_SSE
    __m128 mul = _mm_mul_ps(a.simd, b.simd);
    __m128 sum = _mm_hadd_ps(mul, mul);
    sum = _mm_hadd_ps(sum, sum);
    return _mm_cvtss_f32(sum);
#elif defined(MATHLIB_NEON)
    float32x4_t mul = vmulq_f32(a.simd, b.simd);
    float32x2_t sum = vadd_f32(vget_low_f32(mul), vget_high_f32(mul));
    return vget_lane_f32(vpadd_f32(sum, sum), 0);
#else
    return a.x * b.x + a.y * b.y + a.z * b.z;
#endif
}

static inline vec3_t vec3_cross(vec3_t a, vec3_t b) {
    return (vec3_t){{
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    }};
}

static inline float vec3_length_sq(vec3_t v) {
    return vec3_dot(v, v);
}

static inline float vec3_length(vec3_t v) {
    return sqrtf(vec3_length_sq(v));
}

static inline vec3_t vec3_normalize(vec3_t v) {
    float len = vec3_length(v);
    return len > ML_EPSILON ? vec3_div(v, len) : (vec3_t){{.x = 0, .y = 0, .z = 0, ._pad = 0}};
}

static inline vec3_t vec3_lerp(vec3_t a, vec3_t b, float t) {
    return vec3_add(a, vec3_mul(vec3_sub(b, a), t));
}

static inline float vec3_distance(vec3_t a, vec3_t b) {
    return vec3_length(vec3_sub(b, a));
}

static inline vec3_t vec3_reflect(vec3_t v, vec3_t n) {
    return vec3_sub(v, vec3_mul(n, 2.0f * vec3_dot(v, n)));
}

static inline vec3_t vec3_project(vec3_t v, vec3_t onto) {
    return vec3_mul(onto, vec3_dot(v, onto) / vec3_dot(onto, onto));
}

static inline vec3_t vec3_reject(vec3_t v, vec3_t from) {
    return vec3_sub(v, vec3_project(v, from));
}

float vec3_angle_between(vec3_t a, vec3_t b);
vec3_t vec3_min(vec3_t a, vec3_t b);
vec3_t vec3_max(vec3_t a, vec3_t b);
vec3_t vec3_clamp(vec3_t v, vec3_t min, vec3_t max);
vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);
vec3_t vec3_rotate_axis(vec3_t v, vec3_t axis, float angle);
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t);
bool vec3_nearly_equal(vec3_t a, vec3_t b, float epsilon);

// ============================================================================
// 4D Vector (for homogeneous coordinates)
// ============================================================================

typedef struct {
    union {
        struct { float x, y, z, w; };
        float data[4];
#ifdef MATHLIB_SSE
        __m128 simd;
#endif
#ifdef MATHLIB_NEON
        float32x4_t simd;
#endif
    };
} __attribute__((aligned(16))) vec4_t;

static inline vec4_t vec4_new(float x, float y, float z, float w) {
    return (vec4_t){{.x = x, .y = y, .z = z, .w = w}};
}

static inline vec4_t vec4_from_vec3(vec3_t v, float w) {
    return (vec4_t){{.x = v.x, .y = v.y, .z = v.z, .w = w}};
}

// ============================================================================
// 3x3 Matrix (for 2D transformations)
// ============================================================================

typedef struct {
    float m[9];  // row-major: m00 m01 m02 m10 m11 m12 m20 m21 m22
} mat3_t;

mat3_t mat3_identity(void);
mat3_t mat3_mul(const mat3_t *a, const mat3_t *b);
vec2_t mat3_mul_vec2(const mat3_t *m, vec2_t v);
mat3_t mat3_translate(float x, float y);
mat3_t mat3_rotate(float angle);
mat3_t mat3_scale(float sx, float sy);
mat3_t mat3_inverse(const mat3_t *m);
float mat3_determinant(const mat3_t *m);

// ============================================================================
// 4x4 Matrix (for 3D transformations, column-major for OpenGL)
// ============================================================================

typedef struct {
    union {
        float m[16];  // column-major
        vec4_t cols[4];
    };
} __attribute__((aligned(16))) mat4_t;

mat4_t mat4_identity(void);
mat4_t mat4_mul(const mat4_t *a, const mat4_t *b);
vec4_t mat4_mul_vec4(const mat4_t *m, vec4_t v);
vec3_t mat4_mul_vec3(const mat4_t *m, vec3_t v, float w);
mat4_t mat4_translate(float x, float y, float z);
mat4_t mat4_rotate_x(float angle);
mat4_t mat4_rotate_y(float angle);
mat4_t mat4_rotate_z(float angle);
mat4_t mat4_rotate(float angle, vec3_t axis);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_transpose(const mat4_t *m);
mat4_t mat4_inverse(const mat4_t *m);
float mat4_determinant(const mat4_t *m);

// Camera/projection matrices
mat4_t mat4_look_at(vec3_t eye, vec3_t center, vec3_t up);
mat4_t mat4_perspective(float fov_y, float aspect, float near, float far);
mat4_t mat4_orthographic(float left, float right, float bottom, float top, float near, float far);
mat4_t mat4_isometric(float width, float height, float near, float far);
mat4_t mat4_isometric_view(vec3_t target, float distance);

// ============================================================================
// Quaternion (for smooth rotations)
// ============================================================================

typedef struct {
    union {
        struct { float x, y, z, w; };
        float data[4];
    };
} __attribute__((aligned(16))) quat_t;

static inline quat_t quat_new(float x, float y, float z, float w) {
    return (quat_t){{.x = x, .y = y, .z = z, .w = w}};
}

static inline quat_t quat_identity(void) {
    return (quat_t){{.x = 0, .y = 0, .z = 0, .w = 1}};
}

quat_t quat_from_axis_angle(vec3_t axis, float angle);
quat_t quat_from_euler(float pitch, float yaw, float roll);
quat_t quat_mul(quat_t a, quat_t b);
quat_t quat_conjugate(quat_t q);
quat_t quat_inverse(quat_t q);
quat_t quat_normalize(quat_t q);
float quat_dot(quat_t a, quat_t b);
quat_t quat_slerp(quat_t a, quat_t b, float t);
vec3_t quat_rotate_vec3(quat_t q, vec3_t v);
mat4_t quat_to_mat4(quat_t q);

// ============================================================================
// Utility functions
// ============================================================================

static inline float ml_clamp(float value, float min, float max) {
    return value < min ? min : (value > max ? max : value);
}

static inline float ml_lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

static inline float ml_deg_to_rad(float degrees) {
    return degrees * ML_DEG2RAD;
}

static inline float ml_rad_to_deg(float radians) {
    return radians * ML_RAD2DEG;
}

static inline float ml_smoothstep(float edge0, float edge1, float x) {
    float t = ml_clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

static inline bool ml_nearly_equal(float a, float b, float epsilon) {
    return fabsf(a - b) < epsilon;
}

// Fast inverse square root (Quake III algorithm, still useful)
static inline float ml_rsqrt(float x) {
#ifdef __SSE__
    return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x)));
#else
    union { float f; uint32_t i; } conv = {.f = x};
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5f - (x * 0.5f * conv.f * conv.f);
    return conv.f;
#endif
}

static inline float ml_min(float a, float b) {
    return a < b ? a : b;
}

static inline float ml_max(float a, float b) {
    return a > b ? a : b;
}

static inline float ml_sign(float x) {
    return (x > 0.0f) - (x < 0.0f);
}

static inline float ml_fract(float x) {
    return x - floorf(x);
}

static inline float ml_smootherstep(float edge0, float edge1, float x) {
    float t = ml_clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static inline float ml_inverse_lerp(float a, float b, float value) {
    return (value - a) / (b - a);
}

static inline float ml_remap(float value, float in_min, float in_max, float out_min, float out_max) {
    float t = ml_inverse_lerp(in_min, in_max, value);
    return ml_lerp(out_min, out_max, t);
}

static inline float ml_wrap(float value, float min, float max) {
    float range = max - min;
    return value - range * floorf((value - min) / range);
}

static inline float ml_ping_pong(float t, float length) {
    t = ml_fract(t / (length * 2.0f)) * length * 2.0f;
    return length - fabsf(t - length);
}

static inline int ml_sign_int(float x) {
    return (x > 0.0f) ? 1 : ((x < 0.0f) ? -1 : 0);
}

// ============================================================================
// Bounding volumes
// ============================================================================

typedef struct {
    vec2_t min, max;
} aabb2_t;

typedef struct {
    vec3_t min, max;
} aabb3_t;

typedef struct {
    vec3_t center;
    float radius;
} sphere_t;

// AABB operations
bool aabb2_contains_point(const aabb2_t *box, vec2_t point);
bool aabb2_intersects(const aabb2_t *a, const aabb2_t *b);
bool aabb3_contains_point(const aabb3_t *box, vec3_t point);
bool aabb3_intersects(const aabb3_t *a, const aabb3_t *b);
aabb3_t aabb3_from_points(const vec3_t *points, size_t count);

// Sphere operations
bool sphere_contains_point(const sphere_t *sphere, vec3_t point);
bool sphere_intersects(const sphere_t *a, const sphere_t *b);

// Ray casting
typedef struct {
    vec3_t origin;
    vec3_t direction;
} ray_t;

bool ray_intersects_sphere(const ray_t *ray, const sphere_t *sphere, float *t);
bool ray_intersects_aabb(const ray_t *ray, const aabb3_t *box, float *t);
bool ray_intersects_plane(const ray_t *ray, vec3_t plane_normal, float plane_d, float *t);

// ============================================================================
// Geometry utilities
// ============================================================================

// Triangle operations
float triangle_area(vec3_t a, vec3_t b, vec3_t c);
vec3_t triangle_normal(vec3_t a, vec3_t b, vec3_t c);
vec3_t triangle_centroid(vec3_t a, vec3_t b, vec3_t c);
bool triangle_contains_point(vec3_t a, vec3_t b, vec3_t c, vec3_t p);

// Line segment operations
vec3_t closest_point_on_line_segment(vec3_t a, vec3_t b, vec3_t p);
float distance_point_to_line_segment(vec3_t a, vec3_t b, vec3_t p);

// Plane operations
float plane_distance_to_point(vec3_t plane_normal, float plane_d, vec3_t point);
vec3_t plane_project_point(vec3_t plane_normal, float plane_d, vec3_t point);

// Circle operations (2D)
bool circle_contains_point(vec2_t center, float radius, vec2_t point);
bool circle_intersects_circle(vec2_t center_a, float radius_a, vec2_t center_b, float radius_b);

// Capsule operations
float capsule_distance_to_point_2d(vec2_t a, vec2_t b, float radius, vec2_t p);
float capsule_distance_to_point_3d(vec3_t a, vec3_t b, float radius, vec3_t p);

// Frustum culling
typedef struct {
    vec3_t normal;
    float distance;
} plane_t;

typedef struct {
    plane_t planes[6];
} frustum_t;

frustum_t frustum_from_matrix(const mat4_t *vp);
bool frustum_contains_sphere(const frustum_t *frustum, const sphere_t *sphere);
bool frustum_contains_aabb(const frustum_t *frustum, const aabb3_t *box);

// ============================================================================
// Noise functions (for procedural generation)
// ============================================================================

float noise_value_1d(float x);
float noise_value_2d(float x, float y);
float noise_value_3d(float x, float y, float z);
float noise_fbm_2d(float x, float y, int octaves, float lacunarity, float gain);
float noise_fbm_3d(float x, float y, float z, int octaves, float lacunarity, float gain);
float noise_turbulence_2d(float x, float y, int octaves);
float noise_ridged_2d(float x, float y, int octaves);
float noise_cellular_2d(float x, float y);

#ifdef __cplusplus
}
#endif

#endif // MATHLIB_H
