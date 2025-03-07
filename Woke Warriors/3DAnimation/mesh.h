#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct PBRVertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct PBRTexture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // mesh Data
    vector<PBRVertex>       vertices;
    vector<unsigned int> indices;
    vector<PBRTexture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(vector<PBRVertex> vertices, vector<unsigned int> indices, vector<PBRTexture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    //void Draw(Shader& shader)
    //{
    //    unsigned int diffuseNr = 1;
    //    unsigned int specularNr = 1;
    //    unsigned int normalNr = 1;
    //    unsigned int heightNr = 1;

    //    for (unsigned int i = 0; i < textures.size(); i++) {
    //        glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
    //        stringstream ss;
    //        string number;
    //        string name = textures[i].type;

    //        if (name == "texture_diffuse") {
    //            ss << diffuseNr++; // Transfer unsigned int to stream
    //        }
    //        else if (name == "texture_specular") {
    //            ss << specularNr++; // Transfer unsigned int to stream
    //        }
    //        else if (name == "texture_normal") {
    //            ss << normalNr++; // Transfer unsigned int to stream
    //        }
    //        else if (name == "texture_height") {
    //            ss << heightNr++; // Transfer unsigned int to stream
    //        }
    //        number = ss.str();
    //        shader.setInt(name + number, i);
    //        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    //}

    //// Draw mesh
    //glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0);

    //// Always good practice to set everything back to defaults once configured.
    //glActiveTexture(GL_TEXTURE0);
    //}

    void Draw(Shader& shader) {
        unsigned int albedoNr = 1, normalNr = 1, metallicNr = 1, roughnessNr = 1, aoNr = 1;

        for (unsigned int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i + 3);
            string name = textures[i].type;
            string number;

            if (name == "texture_albedo")
                number = std::to_string(albedoNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);
            else if (name == "texture_metallic")
                number = std::to_string(metallicNr++);
            else if (name == "texture_roughness")
                number = std::to_string(roughnessNr++);
            else if (name == "texture_ao")
                number = std::to_string(aoNr++);

            shader.setInt(name + number, i + 3);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }


private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PBRVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PBRVertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PBRVertex), (void*)offsetof(PBRVertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PBRVertex), (void*)offsetof(PBRVertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(PBRVertex), (void*)offsetof(PBRVertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(PBRVertex), (void*)offsetof(PBRVertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(PBRVertex), (void*)offsetof(PBRVertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(PBRVertex), (void*)offsetof(PBRVertex, m_Weights));
        glBindVertexArray(0);
    }
};
