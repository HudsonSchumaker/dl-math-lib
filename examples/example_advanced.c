/**
 * example_advanced.c - Advanced usage demonstration
 */

#include <stdio.h>
#include "../include/mathlib.h"

void demonstrate_vectors(void) {
    printf("\n=== Vector Operations ===\n");

    vec3_t a = vec3_new(1, 0, 0);
    vec3_t b = vec3_new(0, 1, 0);

    vec3_t cross = vec3_cross(a, b);
    printf("Cross product: (%.2f, %.2f, %.2f)\n", cross.x, cross.y, cross.z);

    float angle = vec3_angle_between(a, b);
    printf("Angle between vectors: %.2f radians (%.2f degrees)\n",
           angle, ml_rad_to_deg(angle));

    vec3_t rotated = vec3_rotate_y(a, ML_PI_2);
    printf("Rotated 90° around Y: (%.2f, %.2f, %.2f)\n",
           rotated.x, rotated.y, rotated.z);
}

void demonstrate_transformations(void) {
    printf("\n=== 3D Transformations ===\n");

    mat4_t translate = mat4_translate(5, 0, 0);
    mat4_t rotate = mat4_rotate_y(ML_PI_4);
    mat4_t scale = mat4_scale(2, 2, 2);

    mat4_t temp = mat4_mul(&translate, &rotate);
    mat4_t transform = mat4_mul(&temp, &scale);

    vec3_t point = vec3_new(1, 0, 0);
    vec3_t transformed = mat4_mul_vec3(&transform, point, 1.0f);

    printf("Original point: (%.2f, %.2f, %.2f)\n", point.x, point.y, point.z);
    printf("Transformed: (%.2f, %.2f, %.2f)\n",
           transformed.x, transformed.y, transformed.z);
}

void demonstrate_quaternions(void) {
    printf("\n=== Quaternion Rotations ===\n");

    quat_t q1 = quat_from_axis_angle(vec3_new(0, 1, 0), 0);
    quat_t q2 = quat_from_axis_angle(vec3_new(0, 1, 0), ML_PI);

    vec3_t point = vec3_new(1, 0, 0);

    for (int i = 0; i <= 4; i++) {
        float t = i / 4.0f;
        quat_t interpolated = quat_slerp(q1, q2, t);
        vec3_t rotated = quat_rotate_vec3(interpolated, point);

        printf("t=%.2f: (%.2f, %.2f, %.2f)\n",
               t, rotated.x, rotated.y, rotated.z);
    }
}

void demonstrate_collision(void) {
    printf("\n=== Collision Detection ===\n");

    sphere_t sphere = {
        .center = vec3_new(0, 0, 0),
        .radius = 5.0f
    };

    aabb3_t box = {
        .min = vec3_new(-2, -2, -2),
        .max = vec3_new(2, 2, 2)
    };

    vec3_t test_point = vec3_new(3, 0, 0);

    printf("Point (3, 0, 0):\n");
    printf("  Inside sphere (r=5): %s\n",
           sphere_contains_point(&sphere, test_point) ? "YES" : "NO");
    printf("  Inside box [-2,2]³: %s\n",
           aabb3_contains_point(&box, test_point) ? "YES" : "NO");

    ray_t ray = {
        .origin = vec3_new(0, 0, -10),
        .direction = vec3_new(0, 0, 1)
    };

    float t;
    if (ray_intersects_sphere(&ray, &sphere, &t)) {
        vec3_t hit_point = vec3_add(ray.origin, vec3_mul(ray.direction, t));
        printf("\nRay hits sphere at: (%.2f, %.2f, %.2f)\n",
               hit_point.x, hit_point.y, hit_point.z);
    }
}

void demonstrate_geometry(void) {
    printf("\n=== Geometry Operations ===\n");

    vec3_t tri_a = vec3_new(0, 0, 0);
    vec3_t tri_b = vec3_new(4, 0, 0);
    vec3_t tri_c = vec3_new(0, 3, 0);

    float area = triangle_area(tri_a, tri_b, tri_c);
    vec3_t normal = triangle_normal(tri_a, tri_b, tri_c);
    vec3_t centroid = triangle_centroid(tri_a, tri_b, tri_c);

    printf("Triangle area: %.2f\n", area);
    printf("Normal: (%.2f, %.2f, %.2f)\n", normal.x, normal.y, normal.z);
    printf("Centroid: (%.2f, %.2f, %.2f)\n", centroid.x, centroid.y, centroid.z);

    vec3_t test = vec3_new(1, 1, 0);
    bool inside = triangle_contains_point(tri_a, tri_b, tri_c, test);
    printf("Point (1, 1, 0) inside triangle: %s\n", inside ? "YES" : "NO");
}

void demonstrate_camera(void) {
    printf("\n=== Camera and Projection ===\n");

    vec3_t eye = vec3_new(0, 5, 10);
    vec3_t center = vec3_new(0, 0, 0);
    vec3_t up = vec3_new(0, 1, 0);

    mat4_t view = mat4_look_at(eye, center, up);
    mat4_t projection = mat4_perspective(
        ml_deg_to_rad(60.0f),  // FOV
        16.0f / 9.0f,           // Aspect ratio
        0.1f,                   // Near plane
        1000.0f                 // Far plane
    );

    mat4_t view_projection = mat4_mul(&projection, &view);

    vec3_t world_point = vec3_new(0, 0, 0);
    vec3_t screen_point = mat4_mul_vec3(&view_projection, world_point, 1.0f);

    printf("World point: (%.2f, %.2f, %.2f)\n",
           world_point.x, world_point.y, world_point.z);
    printf("Screen space: (%.2f, %.2f, %.2f)\n",
           screen_point.x, screen_point.y, screen_point.z);

    frustum_t frustum = frustum_from_matrix(&view_projection);

    sphere_t test_sphere = {
        .center = vec3_new(0, 0, 0),
        .radius = 2.0f
    };

    bool visible = frustum_contains_sphere(&frustum, &test_sphere);
    printf("Sphere at origin visible: %s\n", visible ? "YES" : "NO");
}

void demonstrate_utilities(void) {
    printf("\n=== Utility Functions ===\n");

    printf("Clamp(15, 0, 10) = %.2f\n", ml_clamp(15, 0, 10));
    printf("Lerp(0, 10, 0.5) = %.2f\n", ml_lerp(0, 10, 0.5f));
    printf("Smoothstep(0, 1, 0.5) = %.2f\n", ml_smoothstep(0, 1, 0.5f));
    printf("Smootherstep(0, 1, 0.5) = %.2f\n", ml_smootherstep(0, 1, 0.5f));

    printf("Remap(5, 0, 10, 0, 100) = %.2f\n",
           ml_remap(5, 0, 10, 0, 100));

    printf("Wrap(15, 0, 10) = %.2f\n", ml_wrap(15, 0, 10));
    printf("PingPong(1.5, 1.0) = %.2f\n", ml_ping_pong(1.5f, 1.0f));
}

int main(void) {
    printf("=================================\n");
    printf("MathLib Advanced Examples\n");
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

    demonstrate_vectors();
    demonstrate_transformations();
    demonstrate_quaternions();
    demonstrate_collision();
    demonstrate_geometry();
    demonstrate_camera();
    demonstrate_utilities();

    printf("\n=================================\n");
    printf("All demonstrations complete!\n");
    printf("=================================\n");

    return 0;
}
