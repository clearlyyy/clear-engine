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
        Mesh();
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* mat);
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::vector<Material*> materials);
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::vector<Material*> materials, std::vector<unsigned int> matIndices);
        
        virtual void draw(Shader& shader) const;
        virtual void drawGeometryPass(Shader& shader) const;
        void drawShadowPass(Shader& shader) const;
        
        // Helper method to add material indices
        void setMaterialIndices(const std::vector<unsigned int>& matIndices);
        
        // Getter for materials
        const std::vector<Material*>& getMaterials() const { return materials; }
        
    protected:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Material*> materials;
        std::vector<unsigned int> materialIndices; // One per face (3 indices per face)
        unsigned int VAO, VBO, EBO;

        void setupMesh();
};