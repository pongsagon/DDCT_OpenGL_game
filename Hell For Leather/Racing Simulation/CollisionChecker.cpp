

#include "CollisionChecker.h"

// Custom Min and Max for float
float customMin(float a, float b) {
    return (a < b) ? a : b;
}

float customMax(float a, float b) {
    return (a > b) ? a : b;
}

// Custom Min and Max for int
int customMin(int a, int b) {
    return (a < b) ? a : b;
}

int customMax(int a, int b) {
    return (a > b) ? a : b;
}

template <typename T>
T clamp(T value, T minValue, T maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}


CollisionChecker::CollisionChecker() : gridCells(nullptr), gridCellsCollision(nullptr) {}

void CollisionChecker::setGrid(const std::vector<std::vector<Triangle>>& externalGridCells, const std::vector<std::vector<Triangle>>& externalGridCellsCollision, float gridSize, int gridWidth, int gridHeight) {
    gridCells = &externalGridCells;  // Store pointer to the external grid
    gridCellsCollision = &externalGridCellsCollision;
    this->gridSize = gridSize;
    this->gridWidth = gridWidth;
    this->gridHeight = gridHeight;
}


bool CollisionChecker::checkTrackIntersectionWithGrid(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3& intersectionPoint) {

    if (!gridCells) return false;

    int carGridX = static_cast<int>(std::floor(rayOrigin.x / gridSize));
    int carGridZ = static_cast<int>(std::floor(rayOrigin.z / gridSize));

    carGridX = clamp(carGridX, 0, gridWidth - 1);
    carGridZ = clamp(carGridZ, 0, gridHeight - 1);

    // Convert (x, z) coordinates into the 1D index for the grid cell
    int cellIndex = carGridZ * gridWidth + carGridX;

    const float MAX_FLOAT = 3.402823466e+38F;  // Maximum float value
    float closestT = MAX_FLOAT;
    bool hasIntersection = false;

    // Iterate over the triangles in the target grid cell
    for (const Triangle& tri : (*gridCells)[cellIndex]) {
        float t;
        if (intersectRayWithTriangle(rayOrigin, rayDirection, tri.v0, tri.v1, tri.v2, t)) {
            if (t < closestT) {
                closestT = t;
                intersectionPoint = rayOrigin + rayDirection * t;
                hasIntersection = true;
            }
        }
    }

    return hasIntersection;
}

bool CollisionChecker::checkTrackIntersectionWithGrid(const AABB& aabb) {

    int minGridX = static_cast<int>(std::floor(aabb.min.x / gridSize));
    int maxGridX = static_cast<int>(std::floor(aabb.max.x / gridSize));
    int minGridZ = static_cast<int>(std::floor(aabb.min.z / gridSize));
    int maxGridZ = static_cast<int>(std::floor(aabb.max.z / gridSize));

    minGridX = clamp(minGridX, 0, gridWidth - 1);
    maxGridX = clamp(maxGridX, 0, gridWidth - 1);
    minGridZ = clamp(minGridZ, 0, gridHeight - 1);
    maxGridZ = clamp(maxGridZ, 0, gridHeight - 1);


    for (int x = minGridX; x <= maxGridX; ++x) {
        for (int z = minGridZ; z <= maxGridZ; ++z) {

            int cellIndex = z * gridWidth + x;

            for (const Triangle& tri : (*gridCellsCollision)[cellIndex]) {

                if (intersectAABBWithTriangle(aabb, tri)) {
                    return true;
                }

            }

        }
    }

    return false;  // No collision detected
}

bool CollisionChecker::intersectRayWithTriangle(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float& t) {

    const float EPSILON = 0.0000001f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;

    glm::vec3 h = glm::cross(rayDirection, edge2);
    float a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
        return false;  // Ray is parallel to the triangle

    float f = 1.0f / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f)
        return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDirection, q);
    if (v < 0.0f || u + v > 1.0f)
        return false;

    t = f * glm::dot(edge2, q);
    return t > EPSILON;  // Valid intersection
}

bool CollisionChecker::overlapOnAxis(const glm::vec3& aabbHalfSize, const glm::vec3& axis, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {

    // Ignore small axes to avoid numerical issues
    const float EPSILON = 1e-6f;
    if (glm::length(axis) < EPSILON) return true;

    // Project triangle vertices onto the axis
    float p0 = glm::dot(v0, axis);
    float p1 = glm::dot(v1, axis);
    float p2 = glm::dot(v2, axis);

    // Find the min and max projection values for the triangle
    float triMin = std::min({ p0, p1, p2 });
    float triMax = std::max({ p0, p1, p2 });

    // Project the AABB onto the axis
    float r = aabbHalfSize.x * std::abs(axis.x) + aabbHalfSize.y * std::abs(axis.y) + aabbHalfSize.z * std::abs(axis.z);

    // Check for overlap
    return !(triMin > r || triMax < -r);

}


//SEPERATING AXIS THEOREM


bool CollisionChecker::intersectAABBWithTriangle(const AABB& aabb, const Triangle& tri) {

    glm::vec3 aabbCenter = (aabb.min + aabb.max) * 0.5f;
    glm::vec3 aabbHalfSize = (aabb.max - aabb.min) * 0.5f;

    // Triangle vertices relative to the AABB center
    glm::vec3 v0 = tri.v0 - aabbCenter;
    glm::vec3 v1 = tri.v1 - aabbCenter;
    glm::vec3 v2 = tri.v2 - aabbCenter;

    // Triangle edges
    glm::vec3 f0 = v1 - v0;
    glm::vec3 f1 = v2 - v1;
    glm::vec3 f2 = v0 - v2;

    // 1. Test axes aabbX, aabbY, aabbZ (AABB's local axes)
    if (!overlapOnAxis(aabbHalfSize, glm::vec3(1, 0, 0), v0, v1, v2)) return false;
    if (!overlapOnAxis(aabbHalfSize, glm::vec3(0, 1, 0), v0, v1, v2)) return false;
    if (!overlapOnAxis(aabbHalfSize, glm::vec3(0, 0, 1), v0, v1, v2)) return false;

    // 2. Test axes perpendicular to triangle edges and AABB axes
    if (!overlapOnAxis(aabbHalfSize, glm::cross(f0, glm::vec3(1, 0, 0)), v0, v1, v2)) return false;
    if (!overlapOnAxis(aabbHalfSize, glm::cross(f0, glm::vec3(0, 1, 0)), v0, v1, v2)) return false;
    if (!overlapOnAxis(aabbHalfSize, glm::cross(f0, glm::vec3(0, 0, 1)), v0, v1, v2)) return false;

    if (!overlapOnAxis(aabbHalfSize, glm::cross(f1, glm::vec3(1, 0, 0)), v0, v1, v2)) return false;
    if (!overlapOnAxis(aabbHalfSize, glm::cross(f1, glm::vec3(0, 1, 0)), v0, v1, v2)) return false;
    if (!overlapOnAxis(aabbHalfSize, glm::cross(f1, glm::vec3(0, 0, 1)), v0, v1, v2)) return false;

    if (!overlapOnAxis(aabbHalfSize, glm::cross(f2, glm::vec3(1, 0, 0)), v0, v1, v2)) return false;
    if (!overlapOnAxis(aabbHalfSize, glm::cross(f2, glm::vec3(0, 1, 0)), v0, v1, v2)) return false;
    if (!overlapOnAxis(aabbHalfSize, glm::cross(f2, glm::vec3(0, 0, 1)), v0, v1, v2)) return false;

    // 3. Test the triangle normal axis
    glm::vec3 triangleNormal = glm::normalize(glm::cross(f0, f1));
    if (!overlapOnAxis(aabbHalfSize, triangleNormal, v0, v1, v2)) return false;

    return true;
}
