/**
 * benchmark.c - Performance benchmarking for mathlib
 */

#include <stdio.h>
#include <time.h>
#include "../include/mathlib.h"

#define ITERATIONS 10000000

static double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void benchmark_vec3_ops(void) {
    printf("\n=== Vector Operations (10M iterations) ===\n");

    vec3_t a = vec3_new(1.5f, 2.3f, 3.7f);
    vec3_t b = vec3_new(4.2f, 5.1f, 6.8f);
    vec3_t result;

    double start = get_time();
    for (int i = 0; i < ITERATIONS; i++) {
        result = vec3_add(a, b);
    }
    double elapsed = get_time() - start;
    printf("vec3_add:       %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / ITERATIONS, ITERATIONS / elapsed / 1e6);

    start = get_time();
    for (int i = 0; i < ITERATIONS; i++) {
        result = vec3_mul(a, 2.5f);
    }
    elapsed = get_time() - start;
    printf("vec3_mul:       %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / ITERATIONS, ITERATIONS / elapsed / 1e6);

    float dot_result;
    start = get_time();
    for (int i = 0; i < ITERATIONS; i++) {
        dot_result = vec3_dot(a, b);
    }
    elapsed = get_time() - start;
    printf("vec3_dot:       %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / ITERATIONS, ITERATIONS / elapsed / 1e6);

    start = get_time();
    for (int i = 0; i < ITERATIONS; i++) {
        result = vec3_cross(a, b);
    }
    elapsed = get_time() - start;
    printf("vec3_cross:     %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / ITERATIONS, ITERATIONS / elapsed / 1e6);

    start = get_time();
    for (int i = 0; i < ITERATIONS; i++) {
        result = vec3_normalize(a);
    }
    elapsed = get_time() - start;
    printf("vec3_normalize: %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / ITERATIONS, ITERATIONS / elapsed / 1e6);

    // Prevent optimization
    if (result.x > 1e10f) printf("unlikely\n");
}

void benchmark_mat4_ops(void) {
    printf("\n=== Matrix Operations (1M iterations) ===\n");
    const int iter = ITERATIONS / 10;

    mat4_t a = mat4_translate(1, 2, 3);
    mat4_t b = mat4_rotate_y(0.5f);
    mat4_t result;

    double start = get_time();
    for (int i = 0; i < iter; i++) {
        result = mat4_mul(&a, &b);
    }
    double elapsed = get_time() - start;
    printf("mat4_mul:       %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6);

    vec4_t v = vec4_new(1, 2, 3, 1);
    vec4_t vr;
    start = get_time();
    for (int i = 0; i < iter; i++) {
        vr = mat4_mul_vec4(&a, v);
    }
    elapsed = get_time() - start;
    printf("mat4_mul_vec4:  %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6);

    start = get_time();
    for (int i = 0; i < iter; i++) {
        result = mat4_transpose(&a);
    }
    elapsed = get_time() - start;
    printf("mat4_transpose: %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6);

    start = get_time();
    for (int i = 0; i < iter; i++) {
        result = mat4_inverse(&a);
    }
    elapsed = get_time() - start;
    printf("mat4_inverse:   %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6);

    if (result.m[0] > 1e10f) printf("unlikely\n");
}

void benchmark_quat_ops(void) {
    printf("\n=== Quaternion Operations (1M iterations) ===\n");
    const int iter = ITERATIONS / 10;

    quat_t a = quat_from_axis_angle(vec3_new(0, 1, 0), 0.5f);
    quat_t b = quat_from_axis_angle(vec3_new(1, 0, 0), 0.3f);
    quat_t result;

    double start = get_time();
    for (int i = 0; i < iter; i++) {
        result = quat_mul(a, b);
    }
    double elapsed = get_time() - start;
    printf("quat_mul:       %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6);

    start = get_time();
    for (int i = 0; i < iter; i++) {
        result = quat_normalize(a);
    }
    elapsed = get_time() - start;
    printf("quat_normalize: %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6);

    start = get_time();
    for (int i = 0; i < iter; i++) {
        result = quat_slerp(a, b, 0.5f);
    }
    elapsed = get_time() - start;
    printf("quat_slerp:     %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6);

    vec3_t v = vec3_new(1, 0, 0);
    vec3_t vr;
    start = get_time();
    for (int i = 0; i < iter; i++) {
        vr = quat_rotate_vec3(a, v);
    }
    elapsed = get_time() - start;
    printf("quat_rotate:    %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6);

    if (result.w > 1e10f) printf("unlikely\n");
}

void benchmark_collision(void) {
    printf("\n=== Collision Detection (1M iterations) ===\n");
    const int iter = ITERATIONS / 10;

    sphere_t sphere = {.center = vec3_new(0, 0, 0), .radius = 5.0f};
    ray_t ray = {
        .origin = vec3_new(0, 0, -10),
        .direction = vec3_new(0, 0, 1)
    };
    float t;

    double start = get_time();
    int hits = 0;
    for (int i = 0; i < iter; i++) {
        if (ray_intersects_sphere(&ray, &sphere, &t)) {
            hits++;
        }
    }
    double elapsed = get_time() - start;
    printf("ray-sphere:     %.2f ns/op  (%.2f Mops/sec) [%d hits]\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6, hits);

    aabb3_t box = {
        .min = vec3_new(-2, -2, -2),
        .max = vec3_new(2, 2, 2)
    };

    start = get_time();
    hits = 0;
    for (int i = 0; i < iter; i++) {
        if (ray_intersects_aabb(&ray, &box, &t)) {
            hits++;
        }
    }
    elapsed = get_time() - start;
    printf("ray-aabb:       %.2f ns/op  (%.2f Mops/sec) [%d hits]\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6, hits);

    vec3_t point = vec3_new(1, 1, 1);
    start = get_time();
    int contains = 0;
    for (int i = 0; i < iter; i++) {
        if (sphere_contains_point(&sphere, point)) {
            contains++;
        }
    }
    elapsed = get_time() - start;
    printf("sphere-point:   %.2f ns/op  (%.2f Mops/sec) [%d hits]\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6, contains);
}

void benchmark_noise(void) {
    printf("\n=== Noise Functions (100K iterations) ===\n");
    const int iter = ITERATIONS / 100;

    float result;
    double start = get_time();
    for (int i = 0; i < iter; i++) {
        result = noise_value_2d(i * 0.01f, i * 0.01f);
    }
    double elapsed = get_time() - start;
    printf("noise_value_2d: %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6);

    start = get_time();
    for (int i = 0; i < iter; i++) {
        result = noise_value_3d(i * 0.01f, i * 0.01f, i * 0.01f);
    }
    elapsed = get_time() - start;
    printf("noise_value_3d: %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / iter, iter / elapsed / 1e6);

    start = get_time();
    for (int i = 0; i < iter / 10; i++) {
        result = noise_fbm_2d(i * 0.01f, i * 0.01f, 4, 2.0f, 0.5f);
    }
    elapsed = get_time() - start;
    printf("noise_fbm_2d:   %.2f ns/op  (%.2f Mops/sec)\n",
           elapsed * 1e9 / (iter/10), (iter/10) / elapsed / 1e6);

    if (result > 1e10f) printf("unlikely\n");
}

int main(void) {
    printf("=================================\n");
    printf("MathLib Performance Benchmark\n");
    printf("=================================\n");

#ifdef MATHLIB_SSE
    printf("SIMD: SSE enabled\n");
#elif defined(MATHLIB_AVX)
    printf("SIMD: AVX enabled\n");
#elif defined(MATHLIB_NEON)
    printf("SIMD: NEON enabled\n");
#else
    printf("SIMD: Scalar fallback\n");
#endif

    benchmark_vec3_ops();
    benchmark_mat4_ops();
    benchmark_quat_ops();
    benchmark_collision();
    benchmark_noise();

    printf("\n=================================\n");
    printf("Benchmark complete!\n");
    printf("=================================\n");

    return 0;
}
