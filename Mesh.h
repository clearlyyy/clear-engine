#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "Material.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitanget;
};

class Mesh {
    public:
        std::vector<Material*> materials;
        std::vector<unsigned int> materialIndices; // One per face (3 indices per face)
        
        Mesh();
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* mat);
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::vector<Material*> materials);
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::vector<Material*> materials, std::vector<unsigned int> matIndices);
        
        virtual void draw(Shader& shader) const;
        
        // Helper method to add material indices
        void setMaterialIndices(const std::vector<unsigned int>& matIndices);
        
    protected:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int VAO, VBO, EBO;

        void setupMesh();
};