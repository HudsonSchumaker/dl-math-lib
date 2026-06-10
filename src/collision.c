/**
 * collision.c - Collision detection and bounding volume operations
 */

#include "../include/mathlib.h"

// ============================================================================
// 2D AABB operations
// ============================================================================

bool aabb2_contains_point(const aabb2_t *box, vec2_t point) {
    return point.x >= box->min.x && point.x <= box->max.x &&
           point.y >= box->min.y && point.y <= box->max.y;
}

bool aabb2_intersects(const aabb2_t *a, const aabb2_t *b) {
    return a->min.x <= b->max.x && a->max.x >= b->min.x &&
           a->min.y <= b->max.y && a->max.y >= b->min.y;
}

// ============================================================================
// 3D AABB operations
// ============================================================================

bool aabb3_contains_point(const aabb3_t *box, vec3_t point) {
    return point.x >= box->min.x && point.x <= box->max.x &&
           point.y >= box->min.y && point.y <= box->max.y &&
           point.z >= box->min.z && point.z <= box->max.z;
}

bool aabb3_intersects(const aabb3_t *a, const aabb3_t *b) {
    return a->min.x <= b->max.x && a->max.x >= b->min.x &&
           a->min.y <= b->max.y && a->max.y >= b->min.y &&
           a->min.z <= b->max.z && a->max.z >= b->min.z;
}

aabb3_t aabb3_from_points(const vec3_t *points, size_t count) {
    if (count == 0) {
        return (aabb3_t){
            .min = vec3_new(0, 0, 0),
            .max = vec3_new(0, 0, 0)
        };
    }

    aabb3_t result;
    result.min = points[0];
    result.max = points[0];

    for (size_t i = 1; i < count; i++) {
        if (points[i].x < result.min.x) result.min.x = points[i].x;
        if (points[i].y < result.min.y) result.min.y = points[i].y;
        if (points[i].z < result.min.z) result.min.z = points[i].z;
        if (points[i].x > result.max.x) result.max.x = points[i].x;
        if (points[i].y > result.max.y) result.max.y = points[i].y;
        if (points[i].z > result.max.z) result.max.z = points[i].z;
    }

    return result;
}

// ============================================================================
// Sphere operations
// ============================================================================

bool sphere_contains_point(const sphere_t *sphere, vec3_t point) {
    return vec3_distance(sphere->center, point) <= sphere->radius;
}

bool sphere_intersects(const sphere_t *a, const sphere_t *b) {
    float distance = vec3_distance(a->center, b->center);
    return distance <= (a->radius + b->radius);
}

// ============================================================================
// Ray casting
// ============================================================================

bool ray_intersects_sphere(const ray_t *ray, const sphere_t *sphere, float *t) {
    vec3_t oc = vec3_sub(ray->origin, sphere->center);
    float b = vec3_dot(oc, ray->direction);
    float c = vec3_dot(oc, oc) - sphere->radius * sphere->radius;
    float discriminant = b * b - c;

    if (discriminant < 0) {
        return false;
    }

    float sqrt_discriminant = sqrtf(discriminant);
    float t0 = -b - sqrt_discriminant;
    float t1 = -b + sqrt_discriminant;

    if (t0 > ML_EPSILON) {
        if (t) *t = t0;
        return true;
    }
    if (t1 > ML_EPSILON) {
        if (t) *t = t1;
        return true;
    }

    return false;
}

bool ray_intersects_aabb(const ray_t *ray, const aabb3_t *box, float *t) {
    vec3_t inv_dir = vec3_new(
        1.0f / ray->direction.x,
        1.0f / ray->direction.y,
        1.0f / ray->direction.z
    );

    float t1 = (box->min.x - ray->origin.x) * inv_dir.x;
    float t2 = (box->max.x - ray->origin.x) * inv_dir.x;
    float t3 = (box->min.y - ray->origin.y) * inv_dir.y;
    float t4 = (box->max.y - ray->origin.y) * inv_dir.y;
    float t5 = (box->min.z - ray->origin.z) * inv_dir.z;
    float t6 = (box->max.z - ray->origin.z) * inv_dir.z;

    float tmin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));
    float tmax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));

    if (tmax < 0 || tmin > tmax) {
        return false;
    }

    if (t) *t = tmin > 0 ? tmin : tmax;
    return true;
}

bool ray_intersects_plane(const ray_t *ray, vec3_t plane_normal, float plane_d, float *t) {
    float denom = vec3_dot(ray->direction, plane_normal);

    if (fabsf(denom) > ML_EPSILON) {
        float num = -(vec3_dot(ray->origin, plane_normal) + plane_d);
        float result = num / denom;

        if (result >= 0) {
            if (t) *t = result;
            return true;
        }
    }

    return false;
}
