#pragma once

#include <glm/glm.hpp>
#include <cmath>

struct Ray
{
    glm::vec3 position;
    glm::vec3 direction;
};

struct Triangle
{
    glm::vec3 v1;
    glm::vec3 v2;
    glm::vec3 v3;
};

struct IntersectionResult {
    float Distance;
    glm::vec3 HitLocation;
};

// Optimized ray-triangle intersection function
const float EPSILON = 0.000001f;

static float RayIntersectTriangle(const Ray& ray, const Triangle& triangle, glm::vec3& hitPoint) {

    // Precompute the triangle edges
    glm::vec3 edge1 = triangle.v2 - triangle.v1;
    glm::vec3 edge2 = triangle.v3 - triangle.v1;

    // Begin calculating determinant and auxiliary values
    glm::vec3 pvec = glm::cross(ray.direction, edge2);
    float det = glm::dot(edge1, pvec);

    // If determinant is near zero, ray lies in the plane of the triangle
    if (det > -EPSILON && det < EPSILON) {
        return std::numeric_limits<float>::max();  // No intersection
    }

    float invDet = 1.0f / det;

    // Calculate distance from Vert0 to ray origin
    glm::vec3 tvec = ray.position - triangle.v1;

    // Calculate u parameter and test bounds
    float u = glm::dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f) {
        return std::numeric_limits<float>::max();  // No intersection
    }

    // Prepare to test v parameter
    glm::vec3 qvec = glm::cross(tvec, edge1);

    // Calculate v parameter and test bounds
    float v = glm::dot(ray.direction, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f) {
        return std::numeric_limits<float>::max();  // No intersection
    }

    // Calculate t, the distance along the ray to the intersection
    float t = glm::dot(edge2, qvec) * invDet;

    if (t > EPSILON) {
        // Ray intersection, calculate hit location
        hitPoint = ray.position + ray.direction * t;
        return t;  // Return distance to intersection
    }

    // If t <= EPSILON, no valid intersection
    return std::numeric_limits<float>::max();  // No intersection
}

static glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t)
{
    return a + t * (b - a);
}

static float Lerpf(float start, float end, float t)
{
    return start + t * (end - start);
}

static bool IsNearMe2D(const glm::vec3& Subject, const glm::vec3& Me, float Range)
{
    return (Subject.x > Me.x - Range && Subject.x < Me.x + Range) &&
        (Subject.z > Me.z - Range && Subject.z < Me.z + Range);
}

static float GetYawFromForward(const glm::vec3& m_forward)
{
    glm::vec3 normalizedForward = glm::normalize(m_forward);
    return std::atan2(normalizedForward.x, normalizedForward.z);
}