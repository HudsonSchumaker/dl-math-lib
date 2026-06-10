/**
 * example_isometric.c - Isometric projection demonstration
 *
 * Shows how to set up isometric camera and projection for 2.5D games
 * (like SimCity, Age of Empires, Diablo, etc.)
 */

#include <stdio.h>
#include "../include/mathlib.h"

void print_vec3(const char *name, vec3_t v) {
    printf("%s: (%.3f, %.3f, %.3f)\n", name, v.x, v.y, v.z);
}

void demonstrate_isometric_projection(void) {
    printf("\n=== Isometric Projection Setup ===\n");

    // Set up isometric projection
    // Width and height define the visible area in world units
    float view_width = 20.0f;   // 20 units wide
    float view_height = 15.0f;  // 15 units tall
    float near_plane = 0.1f;
    float far_plane = 100.0f;

    mat4_t projection = mat4_isometric(view_width, view_height, near_plane, far_plane);

    printf("Isometric projection created:\n");
    printf("  View dimensions: %.1f x %.1f units\n", view_width, view_height);
    printf("  Near/Far planes: %.1f / %.1f\n", near_plane, far_plane);

    // Set up isometric view
    vec3_t target = vec3_new(0, 0, 0);  // Looking at origin
    float distance = 20.0f;              // Camera distance from target

    mat4_t view = mat4_isometric_view(target, distance);

    printf("\nIsometric view created:\n");
    printf("  Target: (%.1f, %.1f, %.1f)\n", target.x, target.y, target.z);
    printf("  Distance: %.1f\n", distance);

    // Extract camera position from view matrix
    mat4_t inv_view = mat4_inverse(&view);
    vec3_t camera_pos = vec3_new(inv_view.m[12], inv_view.m[13], inv_view.m[14]);
    print_vec3("  Camera position", camera_pos);

    // Combine view and projection
    mat4_t view_projection = mat4_mul(&projection, &view);

    printf("\nView-Projection matrix created\n");

    // Test: Transform some world points to screen space
    printf("\n=== World to Screen Space ===\n");

    vec3_t world_points[] = {
        vec3_new(0, 0, 0),    // Origin
        vec3_new(1, 0, 0),    // X axis
        vec3_new(0, 1, 0),    // Y axis
        vec3_new(0, 0, 1),    // Z axis
        vec3_new(5, 2, 5),    // Arbitrary point
    };

    const char *point_names[] = {
        "Origin",
        "X+1",
        "Y+1",
        "Z+1",
        "Point(5,2,5)"
    };

    for (int i = 0; i < 5; i++) {
        vec3_t screen = mat4_mul_vec3(&view_projection, world_points[i], 1.0f);
        printf("%-12s: World (%.1f, %.1f, %.1f) -> Screen (%.3f, %.3f, %.3f)\n",
               point_names[i],
               world_points[i].x, world_points[i].y, world_points[i].z,
               screen.x, screen.y, screen.z);
    }
}

void demonstrate_isometric_grid(void) {
    printf("\n=== Isometric Grid Rendering ===\n");

    // Setup isometric camera
    vec3_t target = vec3_new(10, 0, 10);
    float distance = 30.0f;
    mat4_t view = mat4_isometric_view(target, distance);

    mat4_t projection = mat4_isometric(40.0f, 30.0f, 0.1f, 100.0f);
    mat4_t vp = mat4_mul(&projection, &view);

    // Simulate a grid of tiles (like in strategy games)
    printf("Simulating 5x5 tile grid:\n\n");

    for (int z = 0; z < 5; z++) {
        for (int x = 0; x < 5; x++) {
            vec3_t tile_world = vec3_new(x * 2.0f, 0, z * 2.0f);
            vec3_t tile_screen = mat4_mul_vec3(&vp, tile_world, 1.0f);

            // Check if tile is visible (in normalized device coordinates)
            bool visible = (tile_screen.x >= -1.0f && tile_screen.x <= 1.0f &&
                          tile_screen.y >= -1.0f && tile_screen.y <= 1.0f &&
                          tile_screen.z >= -1.0f && tile_screen.z <= 1.0f);

            printf("Tile[%d,%d] screen:(% .2f,% .2f) %s\n",
                   x, z, tile_screen.x, tile_screen.y,
                   visible ? "VISIBLE" : "CULLED");
        }
    }
}

void demonstrate_isometric_depth_sorting(void) {
    printf("\n=== Isometric Depth Sorting ===\n");

    vec3_t camera_dir = vec3_normalize(vec3_new(1, -1, 1));  // Isometric view direction
    printf("Isometric view direction: (%.3f, %.3f, %.3f)\n",
           camera_dir.x, camera_dir.y, camera_dir.z);

    // Objects in the scene
    typedef struct {
        const char *name;
        vec3_t position;
    } object_t;

    object_t objects[] = {
        {"Tree",     vec3_new(5, 0, 5)},
        {"House",    vec3_new(10, 0, 8)},
        {"Rock",     vec3_new(3, 0, 12)},
        {"Player",   vec3_new(8, 0, 10)},
        {"Fountain", vec3_new(12, 0, 15)},
    };

    int count = sizeof(objects) / sizeof(objects[0]);

    // Calculate depth for each object (dot product with view direction)
    // Objects further along the view direction should be rendered first
    printf("\nObjects sorted by depth (back to front):\n");

    // Simple bubble sort by depth
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            float depth_a = vec3_dot(objects[j].position, camera_dir);
            float depth_b = vec3_dot(objects[j + 1].position, camera_dir);

            if (depth_a > depth_b) {
                object_t temp = objects[j];
                objects[j] = objects[j + 1];
                objects[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < count; i++) {
        float depth = vec3_dot(objects[i].position, camera_dir);
        printf("%d. %-10s at (%.1f, %.1f, %.1f) - depth: %.2f\n",
               i + 1, objects[i].name,
               objects[i].position.x, objects[i].position.y, objects[i].position.z,
               depth);
    }

    printf("\nRender order: back to front (painter's algorithm)\n");
}

void demonstrate_isometric_picking(void) {
    printf("\n=== Mouse Picking in Isometric View ===\n");

    // Setup isometric camera
    vec3_t target = vec3_new(10, 0, 10);
    float distance = 30.0f;
    mat4_t view = mat4_isometric_view(target, distance);
    mat4_t projection = mat4_isometric(40.0f, 30.0f, 0.1f, 100.0f);
    mat4_t vp = mat4_mul(&projection, &view);

    // Mouse click at screen coordinates (normalized device coordinates)
    vec2_t mouse_ndc = vec2_new(0.2f, 0.3f);  // Slightly offset from center

    printf("Mouse click at NDC: (%.2f, %.2f)\n", mouse_ndc.x, mouse_ndc.y);

    // Unproject to create a ray
    mat4_t inv_vp = mat4_inverse(&vp);

    vec3_t near_point = mat4_mul_vec3(&inv_vp, vec3_new(mouse_ndc.x, mouse_ndc.y, -1), 1);
    vec3_t far_point = mat4_mul_vec3(&inv_vp, vec3_new(mouse_ndc.x, mouse_ndc.y, 1), 1);

    ray_t picking_ray = {
        .origin = near_point,
        .direction = vec3_normalize(vec3_sub(far_point, near_point))
    };

    print_vec3("Ray origin", picking_ray.origin);
    print_vec3("Ray direction", picking_ray.direction);

    // Test intersection with ground plane (y = 0)
    vec3_t plane_normal = vec3_new(0, 1, 0);
    float plane_d = 0.0f;

    float t;
    if (ray_intersects_plane(&picking_ray, plane_normal, plane_d, &t)) {
        vec3_t hit_point = vec3_add(picking_ray.origin, vec3_mul(picking_ray.direction, t));
        printf("\nGround plane intersection:\n");
        print_vec3("  Hit point", hit_point);
        printf("  Grid tile: [%d, %d]\n",
               (int)floorf(hit_point.x / 2.0f),
               (int)floorf(hit_point.z / 2.0f));
    } else {
        printf("\nNo ground plane intersection\n");
    }
}

void demonstrate_isometric_angles(void) {
    printf("\n=== Isometric Projection Angles ===\n");

    // The classic isometric projection uses these angles:
    float angle_y = ML_PI * 0.25f;  // 45 degrees horizontal rotation
    float angle_x = atanf(1.0f / sqrtf(2.0f));  // ~35.264 degrees vertical rotation

    printf("Standard isometric angles:\n");
    printf("  Horizontal (Y-axis): %.2f° (45°)\n", ml_rad_to_deg(angle_y));
    printf("  Vertical (X-axis):   %.2f° (~35.264°)\n", ml_rad_to_deg(angle_x));

    printf("\nWhy these angles?\n");
    printf("  - 45° rotation makes X and Z axes equally foreshortened\n");
    printf("  - ~35.264° tilt (arctan(1/√2)) ensures equal foreshortening of Y axis\n");
    printf("  - Result: All three axes project at equal lengths\n");

    // Show axis projections
    printf("\nAxis projections in isometric view:\n");

    vec3_t target = vec3_new(0, 0, 0);
    mat4_t view = mat4_isometric_view(target, 10.0f);
    mat4_t projection = mat4_isometric(20.0f, 15.0f, 0.1f, 100.0f);
    mat4_t vp = mat4_mul(&projection, &view);

    vec3_t x_axis = mat4_mul_vec3(&vp, vec3_new(1, 0, 0), 1);
    vec3_t y_axis = mat4_mul_vec3(&vp, vec3_new(0, 1, 0), 1);
    vec3_t z_axis = mat4_mul_vec3(&vp, vec3_new(0, 0, 1), 1);

    printf("  X unit vector -> screen: (%.3f, %.3f)\n", x_axis.x, x_axis.y);
    printf("  Y unit vector -> screen: (%.3f, %.3f)\n", y_axis.x, y_axis.y);
    printf("  Z unit vector -> screen: (%.3f, %.3f)\n", z_axis.x, z_axis.y);

    float x_len = sqrtf(x_axis.x * x_axis.x + x_axis.y * x_axis.y);
    float y_len = sqrtf(y_axis.x * y_axis.x + y_axis.y * y_axis.y);
    float z_len = sqrtf(z_axis.x * z_axis.x + z_axis.y * z_axis.y);

    printf("\n  Projected lengths: X=%.3f, Y=%.3f, Z=%.3f (should be equal)\n",
           x_len, y_len, z_len);
}

int main(void) {
    printf("=================================\n");
    printf("Isometric Projection Examples\n");
    printf("=================================\n");
    printf("\nIsometric projection is commonly used in:\n");
    printf("  - Strategy games (Age of Empires, SimCity)\n");
    printf("  - RPGs (Diablo, Baldur's Gate)\n");
    printf("  - Simulation games (RollerCoaster Tycoon)\n");
    printf("  - CAD and technical drawings\n");

    demonstrate_isometric_angles();
    demonstrate_isometric_projection();
    demonstrate_isometric_grid();
    demonstrate_isometric_depth_sorting();
    demonstrate_isometric_picking();

    printf("\n=================================\n");
    printf("All demonstrations complete!\n");
    printf("=================================\n");

    return 0;
}
