#include "Model.h"

Model::Model()
    : position(0.0f), rotation(0.0f), scale(1.0f) 
{
    dirty = true;
}

Model::Model(glm::vec3 pos, glm::vec3 rot, glm::vec3 modelScale)
    : position(position), rotation(rot), scale(modelScale)
{
    dirty = true;
}

void Model::setPosition(const glm::vec3 &position)
{
    this->position = position;
    dirty = true;
}

void Model::setRotation(const glm::vec3 &rotation)
{
    this->rotation = rotation;
    dirty = true;
}

void Model::setScale(const glm::vec3 &scale)
{
    this->scale = scale;
    dirty = true;
}

void Model::addMesh(const Mesh& mesh)
{
    meshes.push_back(mesh);
}

const std::vector<Mesh>& Model::getMeshes() const {
    return meshes;
}

void Model::updateTransform()
{
    cachedTransform = glm::mat4(1.0f);

    //Translation
    cachedTransform = glm::translate(cachedTransform, position);

    //Rotation
    cachedTransform = glm::rotate(cachedTransform, glm::radians(rotation.x), glm::vec3(1, 0, 0)); // Pitch
    cachedTransform = glm::rotate(cachedTransform, glm::radians(rotation.y), glm::vec3(0, 1, 0)); // Yaw
    cachedTransform = glm::rotate(cachedTransform, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // Roll

    //Scale
    cachedTransform = glm::scale(cachedTransform, scale);
    dirty = false;
}

glm::mat4 Model::getTransform() {
    if (dirty)
        updateTransform();
    return cachedTransform;
}
