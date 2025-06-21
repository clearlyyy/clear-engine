#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Mesh.h"

// Model Class, This manages groups of meshes, that will compile into a Model Object. 
// This holds the a Model Transform, and gets sent to the renderer, where we loop
// through the models list of meshes.
class Model {
    public:
    Model();
    Model(glm::vec3 position, glm::vec3 rotationAxis, glm::vec3 scale);

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

    void updateTransform();


};