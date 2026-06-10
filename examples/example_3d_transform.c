/**
 * example_3d_transform.c - Demonstration of 3D transformations
 */

#include <stdio.h>
#include "../include/mathlib.h"

void print_vec3(const char *label, vec3_t v) {
    printf("%s: (%.3f, %.3f, %.3f)\n", label, v.x, v.y, v.z);
}

void print_mat4(const char *label, const mat4_t *m) {
    printf("%s:\n", label);
    for (int row = 0; row < 4; row++) {
        printf("  [");
        for (int col = 0; col < 4; col++) {
            printf(" %7.3f", m->m[col * 4 + row]);
        }
        printf(" ]\n");
    }
}

int main(void) {
    printf("=================================\n");
    printf("3D Transformations Example\n");
    printf("=================================\n\n");

    vec3_t point = vec3_new(1.0f, 0.0f, 0.0f);
    print_vec3("Original point", point);
    printf("\n");

    mat4_t translation = mat4_translate(5.0f, 3.0f, 2.0f);
    vec3_t translated = mat4_mul_vec3(&translation, point, 1.0f);
    print_vec3("After translation (5, 3, 2)", translated);
    printf("\n");

    mat4_t rotation = mat4_rotate_z(ml_deg_to_rad(90.0f));
    vec3_t rotated = mat4_mul_vec3(&rotation, point, 1.0f);
    print_vec3("After 90° rotation around Z", rotated);
    printf("\n");

    mat4_t scale = mat4_scale(2.0f, 2.0f, 2.0f);
    vec3_t scaled = mat4_mul_vec3(&scale, point, 1.0f);
    print_vec3("After uniform scale (2x)", scaled);
    printf("\n");

    printf("=================================\n");
    printf("Combining Transformations (TRS)\n");
    printf("=================================\n\n");

    mat4_t T = mat4_translate(0.0f, 0.0f, 5.0f);
    mat4_t R = mat4_rotate_y(ml_deg_to_rad(45.0f));
    mat4_t S = mat4_scale(2.0f, 2.0f, 2.0f);

    mat4_t SR = mat4_mul(&S, &R);
    mat4_t TRS = mat4_mul(&T, &SR);

    vec3_t transformed = mat4_mul_vec3(&TRS, point, 1.0f);
    print_vec3("After Scale->Rotate->Translate", transformed);
    print_mat4("\nCombined TRS matrix", &TRS);
    printf("\n");

    printf("=================================\n");
    printf("Camera Transformation\n");
    printf("=================================\n\n");

    vec3_t eye = vec3_new(0.0f, 0.0f, 10.0f);
    vec3_t center = vec3_new(0.0f, 0.0f, 0.0f);
    vec3_t up = vec3_new(0.0f, 1.0f, 0.0f);

    mat4_t view = mat4_look_at(eye, center, up);
    print_mat4("View matrix (look at origin from z=10)", &view);
    printf("\n");

    float fov = ml_deg_to_rad(60.0f);
    float aspect = 16.0f / 9.0f;
    mat4_t projection = mat4_perspective(fov, aspect, 0.1f, 100.0f);
    print_mat4("Perspective projection matrix", &projection);
    printf("\n");

    mat4_t VP = mat4_mul(&projection, &view);
    vec3_t world_point = vec3_new(2.0f, 2.0f, 0.0f);
    vec3_t clip_space = mat4_mul_vec3(&VP, world_point, 1.0f);
    print_vec3("World point (2, 2, 0) in clip space", clip_space);
    printf("\n");

    printf("=================================\n");
    printf("Quaternion Rotations\n");
    printf("=================================\n\n");

    vec3_t axis = vec3_new(0.0f, 1.0f, 0.0f);
    quat_t quat = quat_from_axis_angle(axis, ml_deg_to_rad(90.0f));
    printf("Quaternion for 90° Y-axis rotation: (%.3f, %.3f, %.3f, %.3f)\n",
           quat.x, quat.y, quat.z, quat.w);

    vec3_t v = vec3_new(1.0f, 0.0f, 0.0f);
    vec3_t v_rotated = quat_rotate_vec3(quat, v);
    print_vec3("Vector (1, 0, 0) rotated", v_rotated);
    printf("\n");

    quat_t q1 = quat_identity();
    quat_t q2 = quat_from_axis_angle(vec3_new(0, 1, 0), ml_deg_to_rad(180.0f));
    printf("Spherical interpolation (SLERP):\n");
    for (float t = 0.0f; t <= 1.0f; t += 0.25f) {
        quat_t interpolated = quat_slerp(q1, q2, t);
        vec3_t result = quat_rotate_vec3(interpolated, vec3_new(1, 0, 0));
        printf("  t=%.2f: ", t);
        print_vec3("", result);
    }
    printf("\n");

    mat4_t quat_matrix = quat_to_mat4(quat);
    print_mat4("Quaternion converted to matrix", &quat_matrix);

    return 0;
}
