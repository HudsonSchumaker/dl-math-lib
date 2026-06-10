# Isometric Projection in MathLib

## What is Isometric Projection?

Isometric projection is a method of visually representing three-dimensional objects in two dimensions where the three coordinate axes appear equally foreshortened and the angle between any two of them is 120 degrees.

## Why Isometric?

### Advantages
- **Parallel lines remain parallel** - No perspective distortion
- **Measurements are preserved** - Objects maintain their scale regardless of position
- **Easy depth sorting** - Use painter's algorithm (back-to-front rendering)
- **Simple to implement** - No complex perspective calculations
- **Clear visual representation** - Great for strategy games and technical drawings

### Classic Games Using Isometric View
- **Strategy**: Age of Empires, StarCraft, Civilization II, SimCity
- **RPG**: Diablo I & II, Baldur's Gate, Fallout 1 & 2
- **Simulation**: RollerCoaster Tycoon, Theme Hospital
- **Adventure**: Transistor, Bastion, Hades

## Mathematical Foundation

### Standard Isometric Angles

The classic isometric view uses specific angles to achieve equal foreshortening:

1. **Horizontal Rotation (Y-axis)**: 45° (π/4 radians)
   - Makes X and Z axes equally foreshortened
   
2. **Vertical Rotation (X-axis)**: ~35.264° (arctan(1/√2) radians)
   - Ensures Y axis has the same foreshortening as X and Z
   - This specific angle creates the "true" isometric projection

### Why These Angles?

When you rotate 45° around Y and then ~35.264° around X:
- All three axes project with equal length on screen
- The angle between projected axes is 120° (or 180° - 60°)
- This creates the characteristic "diamond" grid pattern

## API Reference

### Functions

#### `mat4_isometric`
```c
mat4_t mat4_isometric(float width, float height, float near, float far);
```

Creates an orthographic projection matrix suitable for isometric rendering.

**Parameters:**
- `width`: Visible width in world units
- `height`: Visible height in world units
- `near`: Near clipping plane distance
- `far`: Far clipping plane distance

**Returns:** Orthographic projection matrix

**Example:**
```c
// Create isometric projection for a 40x30 unit view
mat4_t projection = mat4_isometric(40.0f, 30.0f, 0.1f, 100.0f);
```

#### `mat4_isometric_view`
```c
mat4_t mat4_isometric_view(vec3_t target, float distance);
```

Creates a view matrix positioned at the standard isometric angles.

**Parameters:**
- `target`: Point in world space the camera looks at
- `distance`: Distance from target to camera position

**Returns:** View matrix for isometric rendering

**Example:**
```c
// Camera looking at origin from 20 units away
vec3_t target = vec3_new(0, 0, 0);
mat4_t view = mat4_isometric_view(target, 20.0f);
```

## Usage Patterns

### Basic Setup

```c
// 1. Create projection matrix
mat4_t projection = mat4_isometric(40.0f, 30.0f, 0.1f, 100.0f);

// 2. Create view matrix
vec3_t target = vec3_new(10, 0, 10);  // Looking at tile (10, 10)
mat4_t view = mat4_isometric_view(target, 30.0f);

// 3. Combine for rendering
mat4_t view_projection = mat4_mul(&projection, &view);

// 4. Transform objects
vec3_t world_pos = vec3_new(5, 0, 5);
vec3_t screen_pos = mat4_mul_vec3(&view_projection, world_pos, 1.0f);
```

### Tile Grid Rendering

```c
// Render a tile-based grid (like in strategy games)
for (int z = 0; z < grid_height; z++) {
    for (int x = 0; x < grid_width; x++) {
        vec3_t tile_pos = vec3_new(x * tile_size, 0, z * tile_size);
        vec3_t screen = mat4_mul_vec3(&view_projection, tile_pos, 1.0f);
        
        // Check if visible (in normalized device coordinates)
        if (screen.x >= -1.0f && screen.x <= 1.0f &&
            screen.y >= -1.0f && screen.y <= 1.0f) {
            // Render tile
        }
    }
}
```

### Depth Sorting (Painter's Algorithm)

```c
// For transparent/overlapping objects, sort by depth
vec3_t camera_dir = vec3_normalize(vec3_new(1, -1, 1));

// Sort objects back-to-front
for (int i = 0; i < object_count; i++) {
    objects[i].depth = vec3_dot(objects[i].position, camera_dir);
}

// Sort by depth (ascending = back to front)
qsort(objects, object_count, sizeof(object_t), compare_depth);

// Render in sorted order
for (int i = 0; i < object_count; i++) {
    render_object(&objects[i]);
}
```

### Mouse Picking

```c
// Convert mouse click to world coordinates
vec2_t mouse_ndc = screen_to_ndc(mouse_x, mouse_y, screen_width, screen_height);

mat4_t inv_vp = mat4_inverse(&view_projection);

vec3_t near_point = mat4_mul_vec3(&inv_vp, 
    vec3_new(mouse_ndc.x, mouse_ndc.y, -1), 1);
vec3_t far_point = mat4_mul_vec3(&inv_vp, 
    vec3_new(mouse_ndc.x, mouse_ndc.y, 1), 1);

ray_t picking_ray = {
    .origin = near_point,
    .direction = vec3_normalize(vec3_sub(far_point, near_point))
};

// Test intersection with ground plane (y = 0)
vec3_t plane_normal = vec3_new(0, 1, 0);
float t;
if (ray_intersects_plane(&picking_ray, plane_normal, 0.0f, &t)) {
    vec3_t hit_point = vec3_add(picking_ray.origin, 
        vec3_mul(picking_ray.direction, t));
    
    // Convert to grid coordinates
    int grid_x = (int)floorf(hit_point.x / tile_size);
    int grid_z = (int)floorf(hit_point.z / tile_size);
}
```

### Camera Scrolling

```c
// Scroll camera to follow player
vec3_t player_pos = get_player_position();

// Smooth camera movement
static vec3_t camera_target = {0};
vec3_t target_diff = vec3_sub(player_pos, camera_target);
camera_target = vec3_add(camera_target, vec3_mul(target_diff, 0.1f));

// Update view matrix
mat4_t view = mat4_isometric_view(camera_target, 30.0f);
```

## Visual Representation

### Coordinate System

In isometric view, the world axes project to screen like this:

```
Screen Space (2D):
        Y
        |
        |
   Z    |    X
    \   |   /
     \  |  /
      \ | /
       \|/
```

**Projected angles between axes**: All 120° apart

### Tile Layout

A 3x3 grid in isometric view:

```
      [0,2]
     /    \
[0,1]      [1,2]
   \      /    \
    [1,1]      [2,2]
   /    \      /
[0,0]   [1,0]
   \    /    \
    [2,0]    [2,1]
```

Each tile is rendered as a diamond/rhombus shape.

## Advanced Techniques

### Multi-Level Terrain

```c
// Render terrain at different heights
for (int z = 0; z < height; z++) {
    for (int x = 0; x < width; x++) {
        float terrain_height = heightmap[z * width + x];
        vec3_t pos = vec3_new(x, terrain_height, z);
        
        // Transform and render
        vec3_t screen = mat4_mul_vec3(&view_projection, pos, 1.0f);
        render_tile(screen, terrain_height);
    }
}
```

### Billboarding (Sprites Always Face Camera)

```c
vec3_t camera_dir = vec3_normalize(vec3_new(1, -1, 1));
vec3_t up = vec3_new(0, 1, 0);
vec3_t right = vec3_normalize(vec3_cross(up, camera_dir));
vec3_t billboard_up = vec3_cross(camera_dir, right);

// Build billboard matrix
mat4_t billboard = mat4_identity();
billboard.m[0] = right.x;
billboard.m[1] = right.y;
billboard.m[2] = right.z;
billboard.m[4] = billboard_up.x;
billboard.m[5] = billboard_up.y;
billboard.m[6] = billboard_up.z;
// ... set position in m[12], m[13], m[14]
```

### Frustum Culling

```c
// Extract frustum from view-projection matrix
frustum_t frustum = frustum_from_matrix(&view_projection);

// Test objects for visibility
for (int i = 0; i < object_count; i++) {
    sphere_t bounds = object_get_bounding_sphere(&objects[i]);
    
    if (frustum_contains_sphere(&frustum, &bounds)) {
        render_object(&objects[i]);
    }
}
```

## Performance Tips

1. **Batch by Depth Layer**: Group objects at same depth to minimize state changes
2. **Use Spatial Partitioning**: Quad-tree or grid for quick culling
3. **Pre-sort Static Objects**: Only sort dynamic objects each frame
4. **Cache Transformations**: Recalculate only when camera or objects move
5. **Use Instancing**: For repeated tiles/objects

## Common Pitfalls

### 1. Z-Fighting
**Problem**: Objects at same depth flicker
**Solution**: Use small depth offset or proper depth sorting

### 2. Sorting Artifacts
**Problem**: Large objects may be incorrectly sorted
**Solution**: Use pivot point (usually bottom center) for depth calculation

### 3. Picking Precision
**Problem**: Mouse picking selects wrong tile
**Solution**: Use tile center for hit testing, not corners

### 4. Screen-Space UI
**Problem**: UI elements distorted by projection
**Solution**: Use separate orthographic projection for UI

## Example Games Architecture

### Strategy Game (RTS)
```
1. Render terrain (sorted by depth)
2. Render units/buildings (sorted by depth)
3. Render effects/particles (sorted by depth)
4. Render UI (separate projection)
```

### Isometric RPG
```
1. Render ground layer
2. Render walls/obstacles (back to front)
3. Render characters/NPCs (sorted by Y+Z position)
4. Render overhead effects
5. Render UI/HUD
```

## Further Reading

- [Isometric Graphics in Video Games (Wikipedia)](https://en.wikipedia.org/wiki/Isometric_video_game_graphics)
- [Axonometric Projection](https://en.wikipedia.org/wiki/Axonometric_projection)
- See [example_isometric.c](examples/example_isometric.c) for complete working examples

## Summary

MathLib provides two simple functions for isometric rendering:
- `mat4_isometric()` - Projection matrix
- `mat4_isometric_view()` - View matrix with correct angles

Combined with the collision detection and geometry utilities, you have everything needed to build classic isometric games!
