#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <GLFW/glfw3.h>
#include <stb_image.h>  // Include stb_image for texture loading

// Structure to hold bone information
struct BoneInfo_DAEstatic {
    unsigned int boneID;
    float weight;
};

// Structure to represent a vertex
struct Vertex_DAEstatic {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    std::vector<BoneInfo_DAEstatic> bones; // Stores bone weights for skeletal animation
};

// Class to handle model loading and drawing for .dae files
class Model_DAEstatic {
public:
    Model_DAEstatic(const std::string& dir);
    void Draw(GLuint shaderID);  // Drawing function, takes shaderID to use for rendering
    void SetBones(const std::vector<glm::mat4>& boneMatrices);  // Set bone matrices for skeletal animation

    const std::vector<Vertex_DAEstatic>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndices() const { return indices; }
    const std::unordered_map<std::string, unsigned int>& GetBoneMapping() const { return boneMapping; }

private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    void loadBones(aiMesh* mesh);
    void setupMesh();
    GLuint loadTexture(const std::string& texturePath);  // Texture loading function

private:
    std::vector<Vertex_DAEstatic> vertices;
    std::vector<unsigned int> indices;
    std::unordered_map<std::string, unsigned int> boneMapping; // Maps bone names to IDs
    std::vector<glm::mat4> boneMatrices;  // Transforms for skeletal animation
    GLuint VAO, VBO, EBO;  // OpenGL buffers for rendering
    std::vector<GLuint> textures;  // Store loaded textures
};

Model_DAEstatic::Model_DAEstatic(const std::string& dir) {
    loadModel(dir);
    setupMesh();
}

void Model_DAEstatic::loadModel(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    processNode(scene->mRootNode, scene);
}

void Model_DAEstatic::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void Model_DAEstatic::processMesh(aiMesh* mesh, const aiScene* scene) {
    unsigned int vertexCount = mesh->mNumVertices;

    // Extract vertices and indices
    for (unsigned int i = 0; i < vertexCount; i++) {
        Vertex_DAEstatic vertex;

        // Positions
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // Normals
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        // Texture coordinates (if available)
        if (mesh->mTextureCoords[0]) {
            vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Extract indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process bones for skeletal animation
    loadBones(mesh);

    // Load textures
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texturePath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
            std::string texPath = texturePath.C_Str();
            GLuint texture = loadTexture(texPath);
            textures.push_back(texture);
        }
    }
}

void Model_DAEstatic::loadBones(aiMesh* mesh) {
    for (unsigned int i = 0; i < mesh->mNumBones; i++) {
        aiBone* bone = mesh->mBones[i];
        std::string boneName = bone->mName.C_Str();
        unsigned int boneID;

        // Create new bone if not found in the map
        if (boneMapping.find(boneName) == boneMapping.end()) {
            boneID = boneMapping.size();
            boneMapping[boneName] = boneID;
        }
        else {
            boneID = boneMapping[boneName];
        }

        // Add bone weights to vertices
        for (unsigned int j = 0; j < bone->mNumWeights; j++) {
            unsigned int vertexIndex = bone->mWeights[j].mVertexId;
            float weight = bone->mWeights[j].mWeight;

            Vertex_DAEstatic& vertex = vertices[vertexIndex];
            vertex.bones.push_back(BoneInfo_DAEstatic{ boneID, weight });
        }
    }
}

GLuint Model_DAEstatic::loadTexture(const std::string& texturePath) {
    int width, height, channels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Texture failed to load at path: " << texturePath << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return textureID;
}

void Model_DAEstatic::SetBones(const std::vector<glm::mat4>& boneMatrices) {
    this->boneMatrices = boneMatrices;
}

void Model_DAEstatic::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Vertex Buffer Object (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex_DAEstatic), &vertices[0], GL_STATIC_DRAW);

    // Element Buffer Object (EBO)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_DAEstatic), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_DAEstatic), (void*)offsetof(Vertex_DAEstatic, normal));
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_DAEstatic), (void*)offsetof(Vertex_DAEstatic, texCoords));
    glEnableVertexAttribArray(2);

    // Bone data (indices and weights)
    glVertexAttribPointer(3, 4, GL_INT, GL_FALSE, sizeof(Vertex_DAEstatic), (void*)offsetof(Vertex_DAEstatic, bones));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_DAEstatic), (void*)(offsetof(Vertex_DAEstatic, bones) + sizeof(int) * 4));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

void Model_DAEstatic::Draw(GLuint shaderID) {
    glBindVertexArray(VAO);

    // Set bone matrices uniform in shader
    GLuint boneMatricesLocation = glGetUniformLocation(shaderID, "boneMatrices");
    glUniformMatrix4fv(boneMatricesLocation, boneMatrices.size(), GL_TRUE, &boneMatrices[0][0][0]);

    // Bind the first texture (if there are any)
    if (!textures.empty()) {
        glBindTexture(GL_TEXTURE_2D, textures[0]);
    }

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}
