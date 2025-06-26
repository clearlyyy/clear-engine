#include "Renderer.h"
#include "GUI.h"

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
            for (const Material* material : mesh.getMaterials()) {
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

void Renderer::render(std::vector<Model*> &models, Camera &camera, Shader* geometryShader)
{
    // Get Matrices for view and projection from the camera
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(WINDOW_WIDTH / WINDOW_HEIGHT);

    // Use the provided geometry shader for the geometry pass
    if (geometryShader) {
        geometryShader->use();
        geometryShader->setMat4("view", view);
        geometryShader->setMat4("projection", projection);
    }

    // Loop through list of models, get each models meshes, set shaders matrices, and draw
    for (Model* model : models) {
        glm::mat4 modelMatrix = model->getTransform();
        
        for (const Mesh& mesh : model->getMeshes()) {
            if (geometryShader) {
                geometryShader->setMat4("model", modelMatrix);
                // Upload per-material shininess for this mesh
                const auto& meshMaterials = mesh.getMaterials();
                for (unsigned int i = 0; i < meshMaterials.size(); ++i) {
                    geometryShader->setFloat("materialShininess[" + std::to_string(i) + "]", meshMaterials[i]->shininess);
                    // Upload per-material specular for this mesh
                    geometryShader->setInt("materialSpecular[" + std::to_string(i) + "]", i);
                    if (meshMaterials[i]->specularMap) {
                        glActiveTexture(GL_TEXTURE0 + i + 32); // Use a different range for specular maps
                        glBindTexture(GL_TEXTURE_2D, meshMaterials[i]->specularMap->ID);
                    }
                }
                mesh.drawGeometryPass(*geometryShader);
            } else {
                // Fallback to regular rendering
                Shader* shader = nullptr;
                for (const Material* material : mesh.getMaterials()) {
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
}