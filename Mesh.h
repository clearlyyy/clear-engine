#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "Material.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
    public:
        Material* material;
        Mesh();
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material);
        virtual void draw(Shader& shader) const;
    protected:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int VAO, VBO, EBO;

        void setupMesh();
};