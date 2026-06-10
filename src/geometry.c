/**
 * geometry.c - Advanced geometric operations
 */

#include "../include/mathlib.h"

// Triangle operations
float triangle_area(vec3_t a, vec3_t b, vec3_t c) {
    vec3_t ab = vec3_sub(b, a);
    vec3_t ac = vec3_sub(c, a);
    return vec3_length(vec3_cross(ab, ac)) * 0.5f;
}

vec3_t triangle_normal(vec3_t a, vec3_t b, vec3_t c) {
    vec3_t ab = vec3_sub(b, a);
    vec3_t ac = vec3_sub(c, a);
    return vec3_normalize(vec3_cross(ab, ac));
}

vec3_t triangle_centroid(vec3_t a, vec3_t b, vec3_t c) {
    return vec3_mul(vec3_add(vec3_add(a, b), c), 1.0f / 3.0f);
}

bool triangle_contains_point(vec3_t a, vec3_t b, vec3_t c, vec3_t p) {
    vec3_t v0 = vec3_sub(c, a);
    vec3_t v1 = vec3_sub(b, a);
    vec3_t v2 = vec3_sub(p, a);

    float dot00 = vec3_dot(v0, v0);
    float dot01 = vec3_dot(v0, v1);
    float dot02 = vec3_dot(v0, v2);
    float dot11 = vec3_dot(v1, v1);
    float dot12 = vec3_dot(v1, v2);

    float inv_denom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
    float v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

// Line segment operations
vec3_t closest_point_on_line_segment(vec3_t a, vec3_t b, vec3_t p) {
    vec3_t ab = vec3_sub(b, a);
    float t = vec3_dot(vec3_sub(p, a), ab) / vec3_dot(ab, ab);
    t = ml_clamp(t, 0.0f, 1.0f);
    return vec3_add(a, vec3_mul(ab, t));
}

float distance_point_to_line_segment(vec3_t a, vec3_t b, vec3_t p) {
    vec3_t closest = closest_point_on_line_segment(a, b, p);
    return vec3_distance(p, closest);
}

// Plane operations
float plane_distance_to_point(vec3_t plane_normal, float plane_d, vec3_t point) {
    return vec3_dot(plane_normal, point) + plane_d;
}

vec3_t plane_project_point(vec3_t plane_normal, float plane_d, vec3_t point) {
    float dist = plane_distance_to_point(plane_normal, plane_d, point);
    return vec3_sub(point, vec3_mul(plane_normal, dist));
}

// Circle operations (2D)
bool circle_contains_point(vec2_t center, float radius, vec2_t point) {
    return vec2_distance(center, point) <= radius;
}

bool circle_intersects_circle(vec2_t center_a, float radius_a, vec2_t center_b, float radius_b) {
    float distance = vec2_distance(center_a, center_b);
    return distance <= (radius_a + radius_b);
}

// Capsule (2D and 3D)
float capsule_distance_to_point_2d(vec2_t a, vec2_t b, float radius, vec2_t p) {
    vec2_t ab = vec2_sub(b, a);
    float t = vec2_dot(vec2_sub(p, a), ab) / vec2_dot(ab, ab);
    t = ml_clamp(t, 0.0f, 1.0f);
    vec2_t closest = vec2_add(a, vec2_mul(ab, t));
    return vec2_distance(p, closest) - radius;
}

float capsule_distance_to_point_3d(vec3_t a, vec3_t b, float radius, vec3_t p) {
    vec3_t closest = closest_point_on_line_segment(a, b, p);
    return vec3_distance(p, closest) - radius;
}

// Frustum culling helpers
frustum_t frustum_from_matrix(const mat4_t *vp) {
    frustum_t frustum;

    // Left plane
    frustum.planes[0].normal.x = vp->m[3] + vp->m[0];
    frustum.planes[0].normal.y = vp->m[7] + vp->m[4];
    frustum.planes[0].normal.z = vp->m[11] + vp->m[8];
    frustum.planes[0].distance = vp->m[15] + vp->m[12];

    // Right plane
    frustum.planes[1].normal.x = vp->m[3] - vp->m[0];
    frustum.planes[1].normal.y = vp->m[7] - vp->m[4];
    frustum.planes[1].normal.z = vp->m[11] - vp->m[8];
    frustum.planes[1].distance = vp->m[15] - vp->m[12];

    // Bottom plane
    frustum.planes[2].normal.x = vp->m[3] + vp->m[1];
    frustum.planes[2].normal.y = vp->m[7] + vp->m[5];
    frustum.planes[2].normal.z = vp->m[11] + vp->m[9];
    frustum.planes[2].distance = vp->m[15] + vp->m[13];

    // Top plane
    frustum.planes[3].normal.x = vp->m[3] - vp->m[1];
    frustum.planes[3].normal.y = vp->m[7] - vp->m[5];
    frustum.planes[3].normal.z = vp->m[11] - vp->m[9];
    frustum.planes[3].distance = vp->m[15] - vp->m[13];

    // Near plane
    frustum.planes[4].normal.x = vp->m[3] + vp->m[2];
    frustum.planes[4].normal.y = vp->m[7] + vp->m[6];
    frustum.planes[4].normal.z = vp->m[11] + vp->m[10];
    frustum.planes[4].distance = vp->m[15] + vp->m[14];

    // Far plane
    frustum.planes[5].normal.x = vp->m[3] - vp->m[2];
    frustum.planes[5].normal.y = vp->m[7] - vp->m[6];
    frustum.planes[5].normal.z = vp->m[11] - vp->m[10];
    frustum.planes[5].distance = vp->m[15] - vp->m[14];

    // Normalize planes
    for (int i = 0; i < 6; i++) {
        float len = vec3_length(frustum.planes[i].normal);
        frustum.planes[i].normal = vec3_div(frustum.planes[i].normal, len);
        frustum.planes[i].distance /= len;
    }

    return frustum;
}

bool frustum_contains_sphere(const frustum_t *frustum, const sphere_t *sphere) {
    for (int i = 0; i < 6; i++) {
        float dist = vec3_dot(frustum->planes[i].normal, sphere->center) + frustum->planes[i].distance;
        if (dist < -sphere->radius) {
            return false;
        }
    }
    return true;
}

bool frustum_contains_aabb(const frustum_t *frustum, const aabb3_t *box) {
    for (int i = 0; i < 6; i++) {
        vec3_t p = box->min;

        if (frustum->planes[i].normal.x >= 0) p.x = box->max.x;
        if (frustum->planes[i].normal.y >= 0) p.y = box->max.y;
        if (frustum->planes[i].normal.z >= 0) p.z = box->max.z;

        if (vec3_dot(frustum->planes[i].normal, p) + frustum->planes[i].distance < 0) {
            return false;
        }
    }
    return true;
}
