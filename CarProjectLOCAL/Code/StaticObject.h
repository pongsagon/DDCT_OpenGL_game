#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>
#include <learnopengl/mesh.h>

#include "Math.h"

class StaticObject
{
public:
    StaticObject(const std::string& modelPath, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
        : m_model(modelPath)
        , m_position(position)
        , m_scale(scale)
        , m_rotation(rotation)
    {
        GenerateMeshTriangles();
    }

    std::vector<Triangle> GetTransformedTriangles() const
    {
        return m_triangles;
    }

    std::vector<Triangle> GetNearTriangles(const glm::vec3& position, float distance) const
    {
        std::vector<Triangle> nearTriangles;

        for (auto& tri : m_triangles)
        {
            if (IsNearMe2D(position, tri.v1, distance))
                nearTriangles.push_back(tri);
        }

        return nearTriangles;
    }

    void GenerateMeshTriangles()
    {
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_position)
            * glm::mat4(glm::quat(m_rotation))
            * glm::scale(glm::mat4(1.0f), m_scale);

        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        for (int i = 0; i < m_model.meshes.size(); i++)
        {
            Mesh& mesh = m_model.meshes[i];
            for (int j = 0; j < mesh.indices.size(); j += 3)
            {
                int v1Index = mesh.indices[j];
                int v2Index = mesh.indices[j + 1];
                int v3Index = mesh.indices[j + 2];

                // Transform the vertices by the model matrix
                glm::vec3 v1 = glm::vec3(modelMatrix * glm::vec4(mesh.vertices[v1Index].Position, 1.0f));
                glm::vec3 v2 = glm::vec3(modelMatrix * glm::vec4(mesh.vertices[v2Index].Position, 1.0f));
                glm::vec3 v3 = glm::vec3(modelMatrix * glm::vec4(mesh.vertices[v3Index].Position, 1.0f));

                // Calculate the normal of the triangle
                glm::vec3 edge1 = v2 - v1;
                glm::vec3 edge2 = v3 - v1;
                glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

                // Check if the dot product of the normal and the up vector is greater than 0
                if (glm::dot(normal, upVector) > 0.0f)
                {
                    // Add the triangle to the m_triangles list
                    m_triangles.push_back({ v1, v2, v3 });
                }
            }
        }
    }


    std::vector<Triangle> m_triangles;

    void Render(Shader& shader)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_position)
            * glm::mat4(glm::quat(m_rotation))
            * glm::scale(glm::mat4(1.0f), glm::vec3(m_scale));

        shader.setMat4("model", model);
        m_model.Draw(shader);
    }

    Model m_model;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_rotation;
};
