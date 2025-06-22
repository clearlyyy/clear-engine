#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <map>
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"

// Model Class, This manages groups of meshes, that will compile into a Model Object. 
// This holds the a Model Transform, and gets sent to the renderer, where we loop
// through the models list of meshes.
class Model {
    public:
    Model();
    Model(glm::vec3 position, glm::vec3 rotationAxis, glm::vec3 scale);

    void loadModel(std::string path);
    void loadModel(std::string path, Shader* shader);
    void addMesh(const Mesh& mesh);
    const std::vector<Mesh>& getMeshes() const;

    void setPosition(const glm::vec3 &position);
    void setRotation(const glm::vec3 &rotation);
    void setScale(const glm::vec3 &scale);
    
    glm::mat4 getTransform();

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    private:

    glm::mat4 cachedTransform;
    bool dirty = true; // True if transform needs updating.

    std::vector<Mesh> meshes;
    std::string directory;
    Shader* modelShader;
    
    // Texture cache to prevent loading the same textures multiple times
    std::map<std::string, Texture*> textureCache;

    void updateTransform();

    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    
    // Helper method to load or get cached texture
    Texture* loadTexture(const std::string& path);

};