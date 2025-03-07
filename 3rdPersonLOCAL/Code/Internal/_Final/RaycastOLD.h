#include "../_Def5.h"




// Ray structure
struct Ray {
    glm::vec3 Origin;
    glm::vec3 Direction;
};

// Triangle structure
struct Triangle {
    glm::vec3 Vert0;
    glm::vec3 Vert1;
    glm::vec3 Vert2;
};

// Structure for intersection results
struct IntersectionResult {
    float Distance;
    glm::vec3 HitLocation;
};

// Function to check for ray-triangle intersection
std::optional<IntersectionResult> RayIntersectTriangle(const Ray& ray, const Triangle& triangle) {
    const float EPSILON = 0.0000001f;

    glm::vec3 edge1 = triangle.Vert1 - triangle.Vert0;
    glm::vec3 edge2 = triangle.Vert2 - triangle.Vert0;

    glm::vec3 h = glm::cross(ray.Direction, edge2);
    float a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON) {
        // This ray is parallel to the triangle.
        return std::nullopt;
    }

    float f = 1.0f / a;
    glm::vec3 s = ray.Origin - triangle.Vert0;
    float u = f * glm::dot(s, h);

    if (u < 0.0f || u > 1.0f) {
        return std::nullopt;
    }

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(ray.Direction, q);

    if (v < 0.0f || u + v > 1.0f) {
        return std::nullopt;
    }

    // At this stage, we can compute the distance t to the intersection point.
    float t = f * glm::dot(edge2, q);

    if (t > EPSILON) {
        // Compute the intersection point
        glm::vec3 hitPoint = ray.Origin + ray.Direction * t;
        return IntersectionResult{ t, hitPoint };
    }
    else {
        return std::nullopt;
    }
}

// Function to find the closest intersection in the scene
std::optional<IntersectionResult> RayIntersectScene(const Ray& ray, const std::vector<Triangle>& triangles) {
    std::optional<IntersectionResult> closestIntersection = std::nullopt;
    float minDistance = std::numeric_limits<float>::max();

    // Loop through all triangles in the scene
    for (const Triangle& triangle : triangles) {
        auto intersection = RayIntersectTriangle(ray, triangle);
        if (intersection && intersection->Distance < minDistance) {
            minDistance = intersection->Distance;
            closestIntersection = intersection;
        }
    }

    return closestIntersection;
}

