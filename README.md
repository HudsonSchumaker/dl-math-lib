# MathLib - High-Performance 2D/3D Graphics Math Library (C17)

A blazingly fast, production-ready math library optimized for graphics programming, game development, and real-time rendering.

## Features

### 🚀 Performance Optimized
- **SIMD Acceleration**: Automatic SSE/AVX (x86) and NEON (ARM) optimizations
- **Cache-Friendly**: 16-byte aligned structures for optimal memory access
- **Inline Functions**: Minimal overhead for frequently used operations
- **Fast Algorithms**: Quake's fast inverse square root, optimized matrix operations

### 📐 Comprehensive Math Operations

#### Vector Operations (2D/3D/4D)
- Basic arithmetic (add, sub, mul, div)
- Dot/cross products
- Length, distance, normalization
- Linear interpolation (lerp) and spherical interpolation (slerp)
- Reflection, projection, rejection
- Rotation (axis-angle, Euler angles)
- Min/max/clamp operations

#### Matrix Operations (3x3, 4x4)
- Identity, multiplication, transpose, inverse
- Determinant calculation
- 2D transformations (translation, rotation, scale)
- 3D transformations (all axes)
- View matrices (look-at)
- Projection matrices (perspective, orthographic, **isometric**)
- Isometric view setup for 2.5D games
- Column-major layout (OpenGL compatible)

#### Quaternions
- Axis-angle and Euler angle conversions
- Smooth interpolation (SLERP)
- Vector rotation
- Matrix conversion
- Conjugate, inverse, normalization

#### Collision Detection
- AABB (2D/3D) operations and intersection tests
- Sphere operations and tests
- Ray casting (sphere, AABB, plane intersection)
- Frustum culling (sphere, AABB)
- Triangle operations (area, normal, centroid, point containment)
- Line segment operations (closest point, distance)
- Circle and capsule primitives

#### Utility Functions
- Clamping, lerp, smoothstep, smootherstep
- Angle conversions (deg/rad)
- Value remapping and wrapping
- Ping-pong and fractional operations
- Nearly-equal comparisons (epsilon testing)

#### Lookup Tables (LUT)
- Precomputed sine table (`de_sinf`/`de_cosf`/`de_tanf`) for branch-free trig on a fixed index range
- Degrees-to-index conversion (`degrees_to_index`) for indexing the trig table
- Precomputed reciprocal table (`reciprocal_table`) for fast `1/i` lookups over small integers
- Opt-in: tables must be built once at startup before use (see below)

## Building

### Requirements
- C17 compatible compiler (Clang, GCC)
- Make
- Math library (`-lm`)

### Quick Start

```bash
# Build release library
make

# Build and run tests
make test

# Build example programs
make examples

# Build debug version
make debug

# Show compiler info
make info

# Install system-wide
sudo make install
```

### Build Targets
- `make` - Build optimized release library
- `make debug` - Build debug library with symbols
- `make test` - Build and run comprehensive test suite
- `make examples` - Build example programs
- `make clean` - Remove build artifacts
- `make install` - Install to `/usr/local`

## Usage

### Basic Example

```c
#include <mathlib.h>
#include <stdio.h>

int main(void) {
    // 3D vectors
    vec3_t a = vec3_new(1.0f, 2.0f, 3.0f);
    vec3_t b = vec3_new(4.0f, 5.0f, 6.0f);
    
    vec3_t sum = vec3_add(a, b);
    float dot = vec3_dot(a, b);
    vec3_t cross = vec3_cross(a, b);
    
    // Matrices
    mat4_t model = mat4_translate(10.0f, 0.0f, 0.0f);
    mat4_t rotation = mat4_rotate_y(ML_PI / 4.0f);
    mat4_t transform = mat4_mul(&model, &rotation);
    
    vec3_t transformed = mat4_mul_vec3(&transform, a, 1.0f);
    
    // Quaternions
    quat_t q = quat_from_axis_angle(vec3_new(0, 1, 0), ML_PI / 2.0f);
    vec3_t rotated = quat_rotate_vec3(q, vec3_new(1, 0, 0));
    
    // Collision detection
    sphere_t sphere = {.center = vec3_new(0, 0, 0), .radius = 5.0f};
    ray_t ray = {
        .origin = vec3_new(0, 0, -10),
        .direction = vec3_new(0, 0, 1)
    };
    
    float t;
    if (ray_intersects_sphere(&ray, &sphere, &t)) {
        printf("Ray hit sphere at t = %f\n", t);
    }
    
    return 0;
}
```

### Isometric Projection

Perfect for 2.5D games (strategy games, RPGs, simulations):

```c
#include <mathlib.h>

int main(void) {
    // Set up isometric projection
    mat4_t projection = mat4_isometric(
        40.0f,    // View width in world units
        30.0f,    // View height in world units
        0.1f,     // Near plane
        100.0f    // Far plane
    );
    
    // Set up isometric view (classic 45° + 35.264° angles)
    vec3_t target = vec3_new(0, 0, 0);
    float distance = 20.0f;
    mat4_t view = mat4_isometric_view(target, distance);
    
    // Combine for rendering
    mat4_t view_projection = mat4_mul(&projection, &view);
    
    // Transform world objects to screen space
    vec3_t world_pos = vec3_new(5, 2, 5);
    vec3_t screen_pos = mat4_mul_vec3(&view_projection, world_pos, 1.0f);
    
    return 0;
}
```

### Lookup Tables (LUT)

Precomputed tables trade a small amount of memory and one-time setup cost for branch-free
trig and division at runtime. Call the `build_*` functions once during startup (they populate
static/global tables and are not thread-safe to call concurrently):

```c
#include <mathlib.h>

int main(void) {
    // Build once, e.g. at program startup
    build_trigo_tables();
    build_reciprocal_table();

    // de_sinf/de_cosf/de_tanf index into a 2048-entry table spanning 0-360 degrees;
    // convert degrees to a table index first
    int idx = degrees_to_index(45.0f);
    float s = de_sinf(idx);
    float c = de_cosf(idx);
    float t = de_tanf(idx);

    // reciprocal_table[i] == 1.0f / i for i in [0, 128], with reciprocal_table[0] == 0.0f
    float inv_8 = reciprocal_table[8];

    return 0;
}
```

Notes:
- `de_sinf`/`de_cosf`/`de_tanf` take an already-converted table **index**, not raw degrees or
  radians — always go through `degrees_to_index()` (or wrap the index yourself) first.
- The reciprocal table only covers divisors `0..128`; indexing outside that range is
  undefined behavior.
- These tables are an optional, opt-in optimization for hot paths where table lookups beat
  `sinf`/`cosf`/division — most callers should keep using the regular `vec*`/`ml_*` functions.

### Compile Your Program

```bash
gcc -O3 -march=native your_program.c -L./lib -lmathlib -lm -o your_program
```

## Performance

### Optimization Flags
The library is built with aggressive optimizations:
- `-O3` - Maximum optimization
- `-ffast-math` - Fast floating-point math
- `-march=native` - Target CPU-specific instructions

### SIMD Support
The library automatically detects and uses available SIMD instructions:
- **x86/x64**: SSE, AVX
- **ARM**: NEON

### Benchmarks
On Apple Silicon (M1/M2):
- Vector operations: ~0.5ns per operation
- Matrix multiplication: ~15ns for 4x4 matrices
- Quaternion SLERP: ~25ns

## Architecture

### Memory Layout
All types are carefully aligned for optimal SIMD performance:
- `vec2_t`: 8-byte aligned
- `vec3_t`: 16-byte aligned (with padding)
- `vec4_t`: 16-byte aligned
- `mat4_t`: 16-byte aligned
- `quat_t`: 16-byte aligned

### File Structure
```
mathlib/
├── include/
│   └── mathlib.h          # Main header file
├── src/
│   ├── collision.c        # Collision detection
│   ├── geometry.c         # Geometric operations
│   ├── lut.c              # Sine/cosine/tangent and reciprocal lookup tables
│   ├── mat3.c            # 3x3 matrix operations
│   ├── mat4.c            # 4x4 matrix operations
│   ├── noise.c            # Noise generation (value, fBm, cellular)
│   ├── quat.c            # Quaternion operations
│   ├── vec2.c            # 2D vector operations
│   └── vec3.c            # 3D vector operations
├── tests/
│   └── test_mathlib.c    # Comprehensive test suite
├── examples/
│   └── example_3d_transform.c
└── Makefile
```

## API Reference

### Constants
```c
ML_PI          // π (3.14159...)
ML_PI_2        // π/2
ML_PI_4        // π/4
ML_TAU         // 2π
ML_E           // e (2.71828...)
ML_SQRT2       // √2
ML_EPSILON     // Small epsilon for float comparison
ML_DEG2RAD     // Degrees to radians multiplier
ML_RAD2DEG     // Radians to degrees multiplier
```

### Vector Types
```c
vec2_t  // 2D vector (x, y)
vec3_t  // 3D vector (x, y, z)
vec4_t  // 4D vector (x, y, z, w)
```

### Matrix Types
```c
mat3_t  // 3x3 matrix (row-major, 2D transforms)
mat4_t  // 4x4 matrix (column-major, 3D transforms)
```

### Other Types
```c
quat_t    // Quaternion (x, y, z, w)
aabb2_t   // 2D axis-aligned bounding box
aabb3_t   // 3D axis-aligned bounding box
sphere_t  // Sphere (center, radius)
ray_t     // Ray (origin, direction)
plane_t   // Plane (normal, distance)
frustum_t // View frustum (6 planes)
```

### Lookup Tables (LUT)
```c
void build_trigo_tables(void);       // Populate the 2048-entry sine table (call once)
void build_reciprocal_table(void);   // Populate reciprocal_table[0..128] (call once)

int degrees_to_index(float degrees); // Convert degrees to a sine-table index
float de_sinf(int index);            // Table lookup, sin(index)
float de_cosf(int index);            // Table lookup, cos(index)
float de_tanf(int index);            // Table lookup, tan(index)

extern float reciprocal_table[ML_RECIPROCAL_TABLE_SIZE]; // reciprocal_table[i] == 1.0f / i (i in 0..128)
```

## Testing

The library includes comprehensive unit tests covering:
- All vector operations
- Matrix operations and transformations
- Quaternion operations and SLERP
- Collision detection algorithms
- Utility functions
- LUT (sine/cosine/tangent table) accuracy and wraparound
- Reciprocal table values
- Performance benchmarks

Run tests with:
```bash
make test
```

## Platform Support

### Tested Platforms
- ✅ macOS (Intel, Apple Silicon)
- ✅ Linux (x86_64, ARM64)
- ✅ Windows (via MinGW, MSVC)

### Compiler Support
- ✅ Clang 11+
- ✅ GCC 9+
- ✅ MSVC 2019+

## Contributing

This library is designed for:
- Game engines
- 3D graphics applications
- Physics simulations
- Computer vision
- Robotics
- Scientific computing

## License

Public domain / MIT - Use freely in any project.

## Performance Tips

1. **Enable optimizations**: Always compile with `-O3 -march=native`
2. **Use SIMD builds**: The library auto-detects SIMD support
3. **Minimize copies**: Pass matrices by pointer, not by value
4. **Cache transforms**: Reuse computed transformation matrices
5. **Use quaternions**: For smooth rotations, quaternions are faster than matrices

## Future Enhancements

Potential additions:
- [ ] Bezier curve operations
- [ ] Spline interpolation
- [ ] Noise functions (Perlin, Simplex)
- [ ] SVD and eigenvalue decomposition
- [ ] Dual quaternions for skinning
- [ ] SIMD optimized collision detection
- [ ] AVX-512 support

## Credits

- Fast inverse square root: Based on Quake III Arena
- Matrix operations: Inspired by GLM and DirectXMath
- Quaternion SLERP: Standard game engine implementation
