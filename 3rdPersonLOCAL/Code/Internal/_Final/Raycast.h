#include "../_Def4.h"




 
// Ray structure
struct B_Ray {
    glm::vec3 Origin;
    glm::vec3 Direction;
};

// Triangle structure
struct B_Triangle {
    glm::vec3 Vert0;
    glm::vec3 Vert1;
    glm::vec3 Vert2;
};



// Optimized ray-triangle intersection function
const float B_EPSILON = 0.000001f;
glm::vec3 edge1;
glm::vec3 edge2;
glm::vec3 pvec;
glm::vec3 tvec;
glm::vec3 qvec;
inline float RayIntersectTriangleOptimized(const B_Ray& ray, const B_Triangle& triangle, glm::vec3& hitPoint) {

    // Precompute the triangle edges
    edge1 = triangle.Vert1 - triangle.Vert0;
    edge2 = triangle.Vert2 - triangle.Vert0;

    // Begin calculating determinant and auxiliary values
     pvec = glm::cross(ray.Direction, edge2);
    float det = glm::dot(edge1, pvec);

    // If determinant is near zero, ray lies in the plane of the triangle
    if (det > -B_EPSILON && det < B_EPSILON) {
        return std::numeric_limits<float>::max();  // No intersection
    }

    float invDet = 1.0f / det;

    // Calculate distance from Vert0 to ray origin
     tvec = ray.Origin - triangle.Vert0;

    // Calculate u parameter and test bounds
    float u = glm::dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f) {
        return std::numeric_limits<float>::max();  // No intersection
    }

    // Prepare to test v parameter
     qvec = glm::cross(tvec, edge1);

    // Calculate v parameter and test bounds
    float v = glm::dot(ray.Direction, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f) {
        return std::numeric_limits<float>::max();  // No intersection
    }

    // Calculate t, the distance along the ray to the intersection
    float t = glm::dot(edge2, qvec) * invDet;

    if (t > B_EPSILON) {
        // Ray intersection, calculate hit location
        hitPoint = ray.Origin + ray.Direction * t;
        return t;  // Return distance to intersection
    }

    // If t <= B_EPSILON, no valid intersection
    return std::numeric_limits<float>::max();  // No intersection
}

glm::vec3 hitPoint;
glm::vec3 PrevHitPoint;
float minDistance;
float R_distance;
bool RayIntersectSceneOptimized(const B_Ray& ray, const std::vector<B_Triangle>& triangles, glm::vec3& closestHitPoint) {
    float minDistance = std::numeric_limits<float>::max();
    bool hitFound = false; 

    // Loop through all triangles in the scene
    for (const B_Triangle& triangle : triangles) {
        float R_distance = RayIntersectTriangleOptimized(ray, triangle, hitPoint);

        // Check if the distance is valid and within range
        if (R_distance > 0.0f && R_distance < minDistance && R_distance <= 5) {
            minDistance = R_distance;
            closestHitPoint = hitPoint;
            hitFound = true;  // Mark a valid hit
        }
    }

    return hitFound;  // Return true if a hit was found within range, false otherwise
}
