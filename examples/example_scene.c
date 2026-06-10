/**
 * example_scene.c - Complete 3D scene example with camera, objects, and culling
 */

#include <stdio.h>
#include "../include/mathlib.h"

typedef struct {
    vec3_t position;
    vec3_t rotation;
    vec3_t scale;
} transform_t;

typedef struct {
    transform_t transform;
    aabb3_t bounds;
    sphere_t bounding_sphere;
} scene_object_t;

mat4_t transform_to_matrix(const transform_t *t) {
    mat4_t translation = mat4_translate(t->position.x, t->position.y, t->position.z);
    mat4_t rotation_x = mat4_rotate_x(t->rotation.x);
    mat4_t rotation_y = mat4_rotate_y(t->rotation.y);
    mat4_t rotation_z = mat4_rotate_z(t->rotation.z);
    mat4_t scale = mat4_scale(t->scale.x, t->scale.y, t->scale.z);

    mat4_t temp1 = mat4_mul(&rotation_z, &rotation_y);
    mat4_t rotation = mat4_mul(&temp1, &rotation_x);
    mat4_t temp2 = mat4_mul(&translation, &rotation);
    return mat4_mul(&temp2, &scale);
}

void print_matrix(const char *name, const mat4_t *m) {
    printf("%s:\n", name);
    for (int row = 0; row < 4; row++) {
        printf("  [");
        for (int col = 0; col < 4; col++) {
            printf(" %7.3f", m->m[col * 4 + row]);
        }
        printf(" ]\n");
    }
}

void demonstrate_scene_setup(void) {
    printf("\n=== Scene Setup ===\n");

    // Camera setup
    vec3_t camera_pos = vec3_new(0, 5, 10);
    vec3_t camera_target = vec3_new(0, 0, 0);
    vec3_t up = vec3_new(0, 1, 0);

    mat4_t view = mat4_look_at(camera_pos, camera_target, up);
    mat4_t projection = mat4_perspective(
        ml_deg_to_rad(60.0f),
        16.0f / 9.0f,
        0.1f,
        1000.0f
    );
    mat4_t view_projection = mat4_mul(&projection, &view);

    printf("Camera position: (%.2f, %.2f, %.2f)\n",
           camera_pos.x, camera_pos.y, camera_pos.z);
    printf("Looking at: (%.2f, %.2f, %.2f)\n",
           camera_target.x, camera_target.y, camera_target.z);
    printf("FOV: 60°, Aspect: 16:9\n");

    // Create frustum for culling
    frustum_t frustum = frustum_from_matrix(&view_projection);
    printf("\nFrustum planes extracted from VP matrix\n");

    // Create some scene objects
    scene_object_t objects[5];

    // Object 0: At origin
    objects[0].transform = (transform_t){
        .position = vec3_new(0, 0, 0),
        .rotation = vec3_new(0, 0, 0),
        .scale = vec3_new(1, 1, 1)
    };
    objects[0].bounding_sphere = (sphere_t){
        .center = objects[0].transform.position,
        .radius = 1.0f
    };

    // Object 1: To the right
    objects[1].transform = (transform_t){
        .position = vec3_new(5, 0, 0),
        .rotation = vec3_new(0, ML_PI_4, 0),
        .scale = vec3_new(2, 2, 2)
    };
    objects[1].bounding_sphere = (sphere_t){
        .center = objects[1].transform.position,
        .radius = 2.0f
    };

    // Object 2: Behind camera (should be culled)
    objects[2].transform = (transform_t){
        .position = vec3_new(0, 0, 20),
        .rotation = vec3_new(0, 0, 0),
        .scale = vec3_new(1, 1, 1)
    };
    objects[2].bounding_sphere = (sphere_t){
        .center = objects[2].transform.position,
        .radius = 1.0f
    };

    // Object 3: Far to the left (should be culled)
    objects[3].transform = (transform_t){
        .position = vec3_new(-50, 0, 0),
        .rotation = vec3_new(0, 0, 0),
        .scale = vec3_new(1, 1, 1)
    };
    objects[3].bounding_sphere = (sphere_t){
        .center = objects[3].transform.position,
        .radius = 1.0f
    };

    // Object 4: In front but slightly left
    objects[4].transform = (transform_t){
        .position = vec3_new(-3, 1, -2),
        .rotation = vec3_new(ML_PI_4, ML_PI_4, 0),
        .scale = vec3_new(1.5f, 1.5f, 1.5f)
    };
    objects[4].bounding_sphere = (sphere_t){
        .center = objects[4].transform.position,
        .radius = 1.5f
    };

    // Frustum culling
    printf("\n=== Frustum Culling ===\n");
    int visible_count = 0;
    for (int i = 0; i < 5; i++) {
        bool visible = frustum_contains_sphere(&frustum, &objects[i].bounding_sphere);
        printf("Object %d at (%.1f, %.1f, %.1f): %s\n",
               i,
               objects[i].transform.position.x,
               objects[i].transform.position.y,
               objects[i].transform.position.z,
               visible ? "VISIBLE" : "CULLED");
        if (visible) visible_count++;
    }
    printf("Total visible: %d / 5\n", visible_count);
}

void demonstrate_transform_hierarchy(void) {
    printf("\n=== Transform Hierarchy ===\n");

    // Parent transform (solar system sun)
    transform_t sun = {
        .position = vec3_new(0, 0, 0),
        .rotation = vec3_new(0, 0, 0),
        .scale = vec3_new(1, 1, 1)
    };

    // Child transform (planet orbiting sun)
    transform_t planet_local = {
        .position = vec3_new(5, 0, 0),  // 5 units from sun
        .rotation = vec3_new(0, ML_PI_4, 0),
        .scale = vec3_new(0.3f, 0.3f, 0.3f)
    };

    // Moon orbiting planet
    transform_t moon_local = {
        .position = vec3_new(1, 0, 0),  // 1 unit from planet
        .rotation = vec3_new(0, 0, 0),
        .scale = vec3_new(0.1f, 0.1f, 0.1f)
    };

    // Compute world matrices
    mat4_t sun_matrix = transform_to_matrix(&sun);
    mat4_t planet_local_matrix = transform_to_matrix(&planet_local);
    mat4_t planet_world_matrix = mat4_mul(&sun_matrix, &planet_local_matrix);

    mat4_t moon_local_matrix = transform_to_matrix(&moon_local);
    mat4_t moon_world_matrix = mat4_mul(&planet_world_matrix, &moon_local_matrix);

    // Extract world positions
    vec3_t sun_world_pos = sun.position;
    vec3_t planet_world_pos = mat4_mul_vec3(&planet_world_matrix, vec3_new(0, 0, 0), 1);
    vec3_t moon_world_pos = mat4_mul_vec3(&moon_world_matrix, vec3_new(0, 0, 0), 1);

    printf("Sun world position:    (%.2f, %.2f, %.2f)\n",
           sun_world_pos.x, sun_world_pos.y, sun_world_pos.z);
    printf("Planet world position: (%.2f, %.2f, %.2f)\n",
           planet_world_pos.x, planet_world_pos.y, planet_world_pos.z);
    printf("Moon world position:   (%.2f, %.2f, %.2f)\n",
           moon_world_pos.x, moon_world_pos.y, moon_world_pos.z);

    float planet_distance = vec3_distance(sun_world_pos, planet_world_pos);
    float moon_distance = vec3_distance(planet_world_pos, moon_world_pos);

    printf("\nDistances:\n");
    printf("Planet from Sun: %.2f\n", planet_distance);
    printf("Moon from Planet: %.2f\n", moon_distance);
}

void demonstrate_picking(void) {
    printf("\n=== Ray Picking ===\n");

    // Mouse click at screen center (normalized device coordinates)
    vec2_t mouse_ndc = vec2_new(0.0f, 0.0f);

    // Camera setup
    vec3_t camera_pos = vec3_new(0, 5, 10);
    vec3_t camera_target = vec3_new(0, 0, 0);
    vec3_t up = vec3_new(0, 1, 0);

    mat4_t view = mat4_look_at(camera_pos, camera_target, up);
    mat4_t projection = mat4_perspective(
        ml_deg_to_rad(60.0f),
        16.0f / 9.0f,
        0.1f,
        1000.0f
    );

    mat4_t view_projection = mat4_mul(&projection, &view);
    mat4_t inv_vp = mat4_inverse(&view_projection);

    // Unproject near and far points
    vec3_t near_point = mat4_mul_vec3(&inv_vp, vec3_new(mouse_ndc.x, mouse_ndc.y, -1), 1);
    vec3_t far_point = mat4_mul_vec3(&inv_vp, vec3_new(mouse_ndc.x, mouse_ndc.y, 1), 1);

    // Create picking ray
    ray_t picking_ray = {
        .origin = camera_pos,
        .direction = vec3_normalize(vec3_sub(far_point, camera_pos))
    };

    printf("Camera position: (%.2f, %.2f, %.2f)\n",
           camera_pos.x, camera_pos.y, camera_pos.z);
    printf("Ray direction: (%.2f, %.2f, %.2f)\n",
           picking_ray.direction.x, picking_ray.direction.y, picking_ray.direction.z);

    // Test objects
    sphere_t objects[] = {
        {.center = vec3_new(0, 0, 0), .radius = 1.0f},
        {.center = vec3_new(3, 0, 0), .radius = 1.0f},
        {.center = vec3_new(-2, 2, 0), .radius = 0.5f},
    };

    printf("\nTesting ray intersection:\n");
    float closest_t = 1e10f;
    int hit_index = -1;

    for (int i = 0; i < 3; i++) {
        float t;
        if (ray_intersects_sphere(&picking_ray, &objects[i], &t)) {
            printf("  Object %d: HIT at t=%.2f\n", i, t);
            if (t < closest_t) {
                closest_t = t;
                hit_index = i;
            }
        } else {
            printf("  Object %d: MISS\n", i);
        }
    }

    if (hit_index >= 0) {
        vec3_t hit_point = vec3_add(
            picking_ray.origin,
            vec3_mul(picking_ray.direction, closest_t)
        );
        printf("\nClosest hit: Object %d at (%.2f, %.2f, %.2f)\n",
               hit_index, hit_point.x, hit_point.y, hit_point.z);
    } else {
        printf("\nNo objects hit\n");
    }
}

void demonstrate_billboarding(void) {
    printf("\n=== Billboard Matrix ===\n");

    vec3_t camera_pos = vec3_new(5, 3, 5);
    vec3_t billboard_pos = vec3_new(0, 2, 0);

    // Calculate direction from billboard to camera
    vec3_t to_camera = vec3_normalize(vec3_sub(camera_pos, billboard_pos));
    vec3_t up = vec3_new(0, 1, 0);

    // Create basis vectors for billboard
    vec3_t right = vec3_normalize(vec3_cross(up, to_camera));
    vec3_t billboard_up = vec3_cross(to_camera, right);

    // Build billboard matrix (world space)
    mat4_t billboard_matrix = mat4_identity();
    billboard_matrix.m[0] = right.x;
    billboard_matrix.m[1] = right.y;
    billboard_matrix.m[2] = right.z;
    billboard_matrix.m[4] = billboard_up.x;
    billboard_matrix.m[5] = billboard_up.y;
    billboard_matrix.m[6] = billboard_up.z;
    billboard_matrix.m[8] = to_camera.x;
    billboard_matrix.m[9] = to_camera.y;
    billboard_matrix.m[10] = to_camera.z;
    billboard_matrix.m[12] = billboard_pos.x;
    billboard_matrix.m[13] = billboard_pos.y;
    billboard_matrix.m[14] = billboard_pos.z;

    printf("Billboard at: (%.2f, %.2f, %.2f)\n",
           billboard_pos.x, billboard_pos.y, billboard_pos.z);
    printf("Camera at: (%.2f, %.2f, %.2f)\n",
           camera_pos.x, camera_pos.y, camera_pos.z);
    printf("Billboard faces camera\n");

    // Transform a point on the billboard
    vec3_t local_point = vec3_new(1, 0, 0);  // Right edge
    vec3_t world_point = mat4_mul_vec3(&billboard_matrix, local_point, 1);
    printf("Local point (1,0,0) -> World (%.2f, %.2f, %.2f)\n",
           world_point.x, world_point.y, world_point.z);
}

int main(void) {
    printf("=================================\n");
    printf("MathLib Complete Scene Example\n");
    printf("=================================\n");

    demonstrate_scene_setup();
    demonstrate_transform_hierarchy();
    demonstrate_picking();
    demonstrate_billboarding();

    printf("\n=================================\n");
    printf("All demonstrations complete!\n");
    printf("=================================\n");

    return 0;
}
