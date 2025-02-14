#ifndef COLLISION_CHECKER_H
#define COLLISION_CHECKER_H

#include <glm/glm.hpp>
#include <vector>

struct Triangle {
    glm::vec3 v0, v1, v2;
};


struct AABB {
    std::vector<glm::vec3> localCorners;  // Predefined corner coordinates
    std::vector<glm::vec3> transformedCorners;  // Transformed corner positions (after applying model matrix)
    glm::vec3 min;  // Minimum corner after transformation
    glm::vec3 max;  // Maximum corner after transformation

    // Constructor: Define the corners relative to the car's local space
    AABB(glm::vec3 halfSize) :
        localCorners(8),
        transformedCorners(8)
    {
        // Initialize the 8 corners in local space (relative to car)
        localCorners = {
            glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z),
            glm::vec3(halfSize.x, -halfSize.y, -halfSize.z),
            glm::vec3(-halfSize.x, halfSize.y, -halfSize.z),
            glm::vec3(halfSize.x, halfSize.y, -halfSize.z),
            glm::vec3(-halfSize.x, -halfSize.y, halfSize.z),
            glm::vec3(halfSize.x, -halfSize.y, halfSize.z),
            glm::vec3(-halfSize.x, halfSize.y, halfSize.z),
            glm::vec3(halfSize.x, halfSize.y, halfSize.z)
        };
    }

    // Update corners by applying the car's model matrix
    void update(const glm::mat4& modelMatrix) {
        for (size_t i = 0; i < localCorners.size(); ++i) {
            // Transform each corner using the model matrix
            glm::vec4 transformed = modelMatrix * glm::vec4(localCorners[i], 1.0f);
            transformedCorners[i] = glm::vec3(transformed);
        }

        // Compute min and max corners from transformed corners
        min = transformedCorners[0];
        max = transformedCorners[0];
        for (const auto& corner : transformedCorners) {
            min = glm::min(min, corner);
            max = glm::max(max, corner);
        }
    }

    // Intersection check between two AABBs
    bool intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
            (min.y <= other.max.y && max.y >= other.min.y) &&
            (min.z <= other.max.z && max.z >= other.min.z);
    }

};


class CollisionChecker {
public:

    CollisionChecker();

    void setGrid(const std::vector<std::vector<Triangle>>& gridCells, const std::vector<std::vector<Triangle>>& gridCellsCollision, float gridSize, int gridWidth, int gridHeight);
    bool checkTrackIntersectionWithGrid(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3& intersectionPoint);
    bool checkTrackIntersectionWithGrid(const AABB& aabb);

private:

    bool overlapOnAxis(const glm::vec3& aabbHalfSize, const glm::vec3& axis, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
    bool intersectRayWithTriangle(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float& t);
    bool intersectAABBWithTriangle(const AABB& aabb, const Triangle& tri);

    const std::vector<std::vector<Triangle>>* gridCells;
    const std::vector<std::vector<Triangle>>* gridCellsCollision;

    int gridSize = 0;
    int gridWidth = 0;
    int gridHeight = 0;

};

#endif