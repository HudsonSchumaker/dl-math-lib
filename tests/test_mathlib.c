/**
 * test_mathlib.c - Comprehensive test suite for mathlib
 */

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "../include/mathlib.h"

#define TEST(name) printf("Testing %s...\n", name)
#define ASSERT_FLOAT_EQ(a, b) assert(fabsf((a) - (b)) < 1e-5f)

void test_vec2(void) {
    TEST("vec2");

    vec2_t a = vec2_new(3, 4);
    vec2_t b = vec2_new(1, 2);

    vec2_t sum = vec2_add(a, b);
    ASSERT_FLOAT_EQ(sum.x, 4);
    ASSERT_FLOAT_EQ(sum.y, 6);

    vec2_t diff = vec2_sub(a, b);
    ASSERT_FLOAT_EQ(diff.x, 2);
    ASSERT_FLOAT_EQ(diff.y, 2);

    float length = vec2_length(a);
    ASSERT_FLOAT_EQ(length, 5.0f);

    vec2_t normalized = vec2_normalize(a);
    ASSERT_FLOAT_EQ(vec2_length(normalized), 1.0f);

    float dot = vec2_dot(a, b);
    ASSERT_FLOAT_EQ(dot, 11.0f);

    vec2_t rotated = vec2_rotate(vec2_new(1, 0), ML_PI_2);
    ASSERT_FLOAT_EQ(rotated.x, 0.0f);
    ASSERT_FLOAT_EQ(rotated.y, 1.0f);

    printf("  ✓ vec2 tests passed\n");
}

void test_vec3(void) {
    TEST("vec3");

    vec3_t a = vec3_new(1, 2, 3);
    vec3_t b = vec3_new(4, 5, 6);

    vec3_t sum = vec3_add(a, b);
    ASSERT_FLOAT_EQ(sum.x, 5);
    ASSERT_FLOAT_EQ(sum.y, 7);
    ASSERT_FLOAT_EQ(sum.z, 9);

    vec3_t diff = vec3_sub(b, a);
    ASSERT_FLOAT_EQ(diff.x, 3);
    ASSERT_FLOAT_EQ(diff.y, 3);
    ASSERT_FLOAT_EQ(diff.z, 3);

    vec3_t scaled = vec3_mul(a, 2.0f);
    ASSERT_FLOAT_EQ(scaled.x, 2);
    ASSERT_FLOAT_EQ(scaled.y, 4);
    ASSERT_FLOAT_EQ(scaled.z, 6);

    float dot = vec3_dot(a, b);
    ASSERT_FLOAT_EQ(dot, 32.0f);

    vec3_t cross = vec3_cross(vec3_new(1, 0, 0), vec3_new(0, 1, 0));
    ASSERT_FLOAT_EQ(cross.x, 0);
    ASSERT_FLOAT_EQ(cross.y, 0);
    ASSERT_FLOAT_EQ(cross.z, 1);

    float length = vec3_length(vec3_new(3, 4, 0));
    ASSERT_FLOAT_EQ(length, 5.0f);

    vec3_t normalized = vec3_normalize(vec3_new(3, 4, 0));
    ASSERT_FLOAT_EQ(vec3_length(normalized), 1.0f);

    printf("  ✓ vec3 tests passed\n");
}

void test_mat3(void) {
    TEST("mat3");

    mat3_t identity = mat3_identity();
    vec2_t v = vec2_new(5, 3);
    vec2_t result = mat3_mul_vec2(&identity, v);
    ASSERT_FLOAT_EQ(result.x, 5);
    ASSERT_FLOAT_EQ(result.y, 3);

    mat3_t translate = mat3_translate(10, 20);
    result = mat3_mul_vec2(&translate, vec2_new(0, 0));
    ASSERT_FLOAT_EQ(result.x, 10);
    ASSERT_FLOAT_EQ(result.y, 20);

    mat3_t scale = mat3_scale(2, 3);
    result = mat3_mul_vec2(&scale, vec2_new(4, 5));
    ASSERT_FLOAT_EQ(result.x, 8);
    ASSERT_FLOAT_EQ(result.y, 15);

    mat3_t rotate = mat3_rotate(ML_PI_2);
    result = mat3_mul_vec2(&rotate, vec2_new(1, 0));
    ASSERT_FLOAT_EQ(result.x, 0.0f);
    ASSERT_FLOAT_EQ(result.y, 1.0f);

    float det = mat3_determinant(&identity);
    ASSERT_FLOAT_EQ(det, 1.0f);

    printf("  ✓ mat3 tests passed\n");
}

void test_mat4(void) {
    TEST("mat4");

    mat4_t identity = mat4_identity();
    vec4_t v = vec4_new(1, 2, 3, 1);
    vec4_t result = mat4_mul_vec4(&identity, v);
    ASSERT_FLOAT_EQ(result.x, 1);
    ASSERT_FLOAT_EQ(result.y, 2);
    ASSERT_FLOAT_EQ(result.z, 3);
    ASSERT_FLOAT_EQ(result.w, 1);

    mat4_t translate = mat4_translate(5, 10, 15);
    vec3_t v3 = vec3_new(0, 0, 0);
    vec3_t result3 = mat4_mul_vec3(&translate, v3, 1.0f);
    ASSERT_FLOAT_EQ(result3.x, 5);
    ASSERT_FLOAT_EQ(result3.y, 10);
    ASSERT_FLOAT_EQ(result3.z, 15);

    mat4_t scale = mat4_scale(2, 3, 4);
    result3 = mat4_mul_vec3(&scale, vec3_new(1, 1, 1), 1.0f);
    ASSERT_FLOAT_EQ(result3.x, 2);
    ASSERT_FLOAT_EQ(result3.y, 3);
    ASSERT_FLOAT_EQ(result3.z, 4);

    mat4_t rot_x = mat4_rotate_x(ML_PI_2);
    result3 = mat4_mul_vec3(&rot_x, vec3_new(0, 1, 0), 1.0f);
    ASSERT_FLOAT_EQ(result3.x, 0.0f);
    ASSERT_FLOAT_EQ(result3.y, 0.0f);
    ASSERT_FLOAT_EQ(result3.z, 1.0f);

    mat4_t a = mat4_translate(1, 2, 3);
    mat4_t b = mat4_scale(2, 2, 2);
    mat4_t c = mat4_mul(&a, &b);
    result3 = mat4_mul_vec3(&c, vec3_new(1, 1, 1), 1.0f);
    ASSERT_FLOAT_EQ(result3.x, 3);
    ASSERT_FLOAT_EQ(result3.y, 4);
    ASSERT_FLOAT_EQ(result3.z, 5);

    float det = mat4_determinant(&identity);
    ASSERT_FLOAT_EQ(det, 1.0f);

    mat4_t transposed = mat4_transpose(&identity);
    ASSERT_FLOAT_EQ(transposed.m[0], 1);
    ASSERT_FLOAT_EQ(transposed.m[5], 1);
    ASSERT_FLOAT_EQ(transposed.m[10], 1);
    ASSERT_FLOAT_EQ(transposed.m[15], 1);

    printf("  ✓ mat4 tests passed\n");
}

void test_quat(void) {
    TEST("quat");

    quat_t identity = quat_identity();
    ASSERT_FLOAT_EQ(identity.w, 1.0f);
    ASSERT_FLOAT_EQ(identity.x, 0.0f);

    vec3_t axis = vec3_new(0, 0, 1);
    quat_t q = quat_from_axis_angle(axis, ML_PI_2);
    vec3_t v = vec3_new(1, 0, 0);
    vec3_t rotated = quat_rotate_vec3(q, v);
    ASSERT_FLOAT_EQ(rotated.x, 0.0f);
    ASSERT_FLOAT_EQ(rotated.y, 1.0f);
    ASSERT_FLOAT_EQ(rotated.z, 0.0f);

    quat_t q1 = quat_from_axis_angle(vec3_new(0, 0, 1), ML_PI_4);
    quat_t q2 = quat_from_axis_angle(vec3_new(0, 0, 1), ML_PI_4);
    quat_t q3 = quat_mul(q1, q2);
    vec3_t v2 = quat_rotate_vec3(q3, vec3_new(1, 0, 0));
    ASSERT_FLOAT_EQ(v2.x, 0.0f);
    ASSERT_FLOAT_EQ(v2.y, 1.0f);

    quat_t normalized = quat_normalize(q);
    float len = sqrtf(quat_dot(normalized, normalized));
    ASSERT_FLOAT_EQ(len, 1.0f);

    quat_t qa = quat_identity();
    quat_t qb = quat_from_axis_angle(vec3_new(0, 1, 0), ML_PI);
    quat_t slerped = quat_slerp(qa, qb, 0.5f);
    vec3_t v3 = quat_rotate_vec3(slerped, vec3_new(1, 0, 0));
    ASSERT_FLOAT_EQ(v3.y, 0.0f);

    printf("  ✓ quat tests passed\n");
}

void test_collision(void) {
    TEST("collision");

    aabb2_t box2 = {
        .min = vec2_new(-1, -1),
        .max = vec2_new(1, 1)
    };
    assert(aabb2_contains_point(&box2, vec2_new(0, 0)));
    assert(!aabb2_contains_point(&box2, vec2_new(2, 2)));

    aabb2_t box2_a = {.min = vec2_new(0, 0), .max = vec2_new(2, 2)};
    aabb2_t box2_b = {.min = vec2_new(1, 1), .max = vec2_new(3, 3)};
    assert(aabb2_intersects(&box2_a, &box2_b));

    aabb2_t box2_c = {.min = vec2_new(10, 10), .max = vec2_new(12, 12)};
    assert(!aabb2_intersects(&box2_a, &box2_c));

    aabb3_t box3 = {
        .min = vec3_new(-1, -1, -1),
        .max = vec3_new(1, 1, 1)
    };
    assert(aabb3_contains_point(&box3, vec3_new(0, 0, 0)));
    assert(!aabb3_contains_point(&box3, vec3_new(2, 2, 2)));

    sphere_t sphere = {
        .center = vec3_new(0, 0, 0),
        .radius = 5.0f
    };
    assert(sphere_contains_point(&sphere, vec3_new(3, 0, 0)));
    assert(!sphere_contains_point(&sphere, vec3_new(10, 0, 0)));

    ray_t ray = {
        .origin = vec3_new(0, 0, -10),
        .direction = vec3_new(0, 0, 1)
    };
    float t;
    assert(ray_intersects_sphere(&ray, &sphere, &t));
    assert(t > 0);

    ray_t ray2 = {
        .origin = vec3_new(10, 10, 10),
        .direction = vec3_new(0, 0, 1)
    };
    assert(!ray_intersects_sphere(&ray2, &sphere, &t));

    assert(ray_intersects_aabb(&ray, &box3, &t));
    assert(t > 0);

    vec3_t plane_normal = vec3_new(0, 1, 0);
    float plane_d = 0.0f;
    ray_t ray3 = {
        .origin = vec3_new(0, 10, 0),
        .direction = vec3_new(0, -1, 0)
    };
    assert(ray_intersects_plane(&ray3, plane_normal, plane_d, &t));
    ASSERT_FLOAT_EQ(t, 10.0f);

    printf("  ✓ collision tests passed\n");
}

void test_utilities(void) {
    TEST("utilities");

    ASSERT_FLOAT_EQ(ml_clamp(5, 0, 10), 5);
    ASSERT_FLOAT_EQ(ml_clamp(-5, 0, 10), 0);
    ASSERT_FLOAT_EQ(ml_clamp(15, 0, 10), 10);

    ASSERT_FLOAT_EQ(ml_lerp(0, 10, 0.5f), 5);
    ASSERT_FLOAT_EQ(ml_lerp(0, 10, 0.0f), 0);
    ASSERT_FLOAT_EQ(ml_lerp(0, 10, 1.0f), 10);

    float deg = 180.0f;
    float rad = ml_deg_to_rad(deg);
    ASSERT_FLOAT_EQ(rad, ML_PI);
    ASSERT_FLOAT_EQ(ml_rad_to_deg(rad), deg);

    assert(ml_nearly_equal(1.0f, 1.00001f, 0.001f));
    assert(!ml_nearly_equal(1.0f, 1.1f, 0.01f));

    float smoothed = ml_smoothstep(0, 1, 0.5f);
    assert(smoothed > 0.4f && smoothed < 0.6f);

    float rsqrt = ml_rsqrt(4.0f);
    assert(fabsf(rsqrt - 0.5f) < 0.01f);

    printf("  ✓ utility tests passed\n");
}

void benchmark_operations(void) {
    TEST("performance benchmark");

    const int iterations = 1000000;

    vec3_t a = vec3_new(1, 2, 3);
    vec3_t b = vec3_new(4, 5, 6);
    vec3_t result;

    for (int i = 0; i < iterations; i++) {
        result = vec3_add(a, b);
        result = vec3_mul(result, 1.01f);
        result = vec3_normalize(result);
    }

    mat4_t m1 = mat4_identity();
    mat4_t m2 = mat4_rotate_y(0.1f);
    mat4_t m_result;

    for (int i = 0; i < iterations / 100; i++) {
        m_result = mat4_mul(&m1, &m2);
        m1 = m_result;
    }

    printf("  ✓ Completed %d iterations\n", iterations);
}

int main(void) {
    printf("=================================\n");
    printf("mathlib Test Suite\n");
    printf("=================================\n\n");

#ifdef MATHLIB_SSE
    printf("SIMD: SSE enabled\n");
#elif defined(MATHLIB_AVX)
    printf("SIMD: AVX enabled\n");
#elif defined(MATHLIB_NEON)
    printf("SIMD: NEON enabled\n");
#else
    printf("SIMD: Scalar fallback\n");
#endif
    printf("\n");

    test_vec2();
    test_vec3();
    test_mat3();
    test_mat4();
    test_quat();
    test_collision();
    test_utilities();
    benchmark_operations();

    printf("\n=================================\n");
    printf("✓ All tests passed!\n");
    printf("=================================\n");

    return 0;
}
