#include "Mesh.h"

Mesh::Mesh() {}

Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& inds, Material* mat)
    : vertices(verts), indices(inds)
{
    materials.push_back(mat);
    // Initialize material indices for single material (all faces use material 0)
    materialIndices.resize(inds.size() / 3, 0); // One index per face (3 indices per face)
    setupMesh();
}

Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& inds, std::vector<Material*> mats)
    : vertices(verts), indices(inds), materials(mats)
{
    // Initialize material indices for single material (all faces use material 0)
    materialIndices.resize(inds.size() / 3, 0); // One index per face
    setupMesh();
}

Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& inds, std::vector<Material*> mats, std::vector<unsigned int> matIndices)
    : vertices(verts), indices(inds), materials(mats), materialIndices(matIndices)
{
    setupMesh();
}

void Mesh::setMaterialIndices(const std::vector<unsigned int>& matIndices) {
    materialIndices = matIndices;
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    //Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    //Tex Coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::draw(Shader& shader) const {
    glBindVertexArray(VAO);
    
    // If we have material indices, draw faces grouped by material
    if (!materialIndices.empty() && materials.size() > 1) {
        // Group faces by material and draw them together
        for (unsigned int materialIndex = 0; materialIndex < materials.size(); materialIndex++) {
            if (materials[materialIndex]) {
                // Apply the material (this will bind textures and set uniforms)
                materials[materialIndex]->Apply();
                
                // Collect indices for faces that use this material
                std::vector<unsigned int> materialIndicesList;
                for (unsigned int faceIndex = 0; faceIndex < materialIndices.size(); faceIndex++) {
                    if (materialIndices[faceIndex] == materialIndex) {
                        // Add the 3 indices for this face
                        unsigned int baseIndex = faceIndex * 3;
                        materialIndicesList.push_back(indices[baseIndex]);
                        materialIndicesList.push_back(indices[baseIndex + 1]);
                        materialIndicesList.push_back(indices[baseIndex + 2]);
                    }
                }
                
                // Draw faces for this material
                if (!materialIndicesList.empty()) {
                    glDrawElements(GL_TRIANGLES, materialIndicesList.size(), GL_UNSIGNED_INT, materialIndicesList.data());
                }
            }
        }
    } else {
        // Fallback: draw entire mesh with first material (or no material)
        if (!materials.empty() && materials[0]) {
            materials[0]->Apply();
        }
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
    
    glBindVertexArray(0);
}