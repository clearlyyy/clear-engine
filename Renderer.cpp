#include "Renderer.h"

Renderer::Renderer()
{
    std::cout << "Renderer Object Created" << std::endl;
}

void Renderer::render(std::vector<Model*> &models, Camera &camera)
{
    // Get Matrices for view and projection from the camera
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(WINDOW_WIDTH / WINDOW_HEIGHT);

    // Loop through list of models, get each models meshes, set shaders matrices, and draw
    for (Model* model : models) {
        glm::mat4 modelMatrix = model->getTransform();
        
        for (const Mesh& mesh : model->getMeshes()) {
            // Find the first valid material to get the shader
            Shader* shader = nullptr;
            for (const Material* material : mesh.materials) {
                if (material && material->shader) {
                    shader = material->shader;
                    break;
                }
            }
            
            if (shader) {
                shader->use();
                shader->setMat4("model", modelMatrix);
                shader->setMat4("view", view);
                shader->setMat4("projection", projection);
                
                mesh.draw(*shader);
            } else {
                std::cout << "Warning: Mesh has no valid material with shader" << std::endl;
            }
        }
    }
}