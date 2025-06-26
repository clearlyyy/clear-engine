#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <fstream>

// Static counter for unique material indices across all models
static int globalMaterialIndexCounter = 0;

Model::Model()
    : position(0.0f), rotation(0.0f), scale(1.0f), modelShader(nullptr)
{
    dirty = true;
}

Model::Model(glm::vec3 pos, glm::vec3 rot, glm::vec3 modelScale)
    : position(pos), rotation(rot), scale(modelScale), modelShader(nullptr)
{
    dirty = true;
}

void Model::setPosition(const glm::vec3 &position)
{
    this->position = position;
    dirty = true;
}

void Model::setName(const std::string& name)
{
    this->name = name;
}

void Model::setRotation(const glm::vec3 &rotation)
{
    this->rotation = rotation;
    dirty = true;
}

void Model::loadModel(std::string path)
{
    loadModel(path, nullptr);
}

void Model::loadModel(std::string path, Shader* shader)
{
    std::cout << "Loading Model at path: " << path << std::endl;
    modelShader = shader;

    Assimp::Importer importer;
    
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ASSIMP ERROR" << importer.GetErrorString() << std::endl;
        return;
    }
    
    directory = path.substr(0, path.find_last_of('/'));
    
    processNode(scene->mRootNode, scene);
    std::cout << "Model loading complete!" << std::endl;
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    //process all the nodes meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    //Then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Texture* Model::loadTexture(const std::string& path)
{
    // Check if texture is already loaded
    auto it = textureCache.find(path);
    if (it != textureCache.end()) {
        // If we have a cached texture, return it (even if it failed)
        return it->second;
    }
    
    // Load new texture
    Texture* texture = new Texture(path.c_str(), true);
    
    // Cache the texture (even if it failed, to prevent repeated attempts)
    textureCache[path] = texture;
    
    return texture;
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Material*> materials;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm::vec3 vector;

        // positions;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        //normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        //Texture Coordinates;
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;

            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            // Check if tangents exist before accessing them
            if (mesh->mTangents)
            {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
            }
            else
            {
                vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            }
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    
    //Now walk through each of the mesh's faces and retrieve the corresponding indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
        
        // Create a new Material
        Material* material = new Material();
        // Use a more reasonable indexing scheme - start from 1 to avoid conflicts with cube
        material->materialIndex = 1 + (globalMaterialIndexCounter % 15); // Keep within 1-15 range
        globalMaterialIndexCounter++;
        material->shader = modelShader; // Assign the model's shader
        
        // Load diffuse texture using cache
        aiString diffusePath;
        if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath) == AI_SUCCESS)
        {
            std::string fullPath = directory + "/" + std::string(diffusePath.C_Str());
            
            // Check if file exists
            std::ifstream fileCheck(fullPath);
            if (fileCheck.good()) {
                fileCheck.close();
                Texture* diffuseTex = loadTexture(fullPath);
                if (diffuseTex && diffuseTex->ID != 0) {
                    material->diffuseMap = diffuseTex;
                    // Check if texture has alpha channel (4 channels = RGBA)
                    if (diffuseTex->getChannels() == 4) {
                        material->hasTransparency = true;
                    }
                } else {
                    material->diffuseMap = nullptr;
                    std::cout << "Failed to load diffuse texture: " << fullPath << std::endl;
                }
            } else {
                material->diffuseMap = nullptr;
                std::cout << "Diffuse texture file does not exist: " << fullPath << std::endl;
            }
        } else {
            std::cout << "No diffuse texture found in material" << std::endl;
        }
        
        // Load specular texture using cache
        aiString specularPath;
        if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &specularPath) == AI_SUCCESS)
        {
            std::string fullPath = directory + "/" + std::string(specularPath.C_Str());
            
            // Check if file exists
            std::ifstream fileCheck(fullPath);
            if (fileCheck.good()) {
                fileCheck.close();
                Texture* specularTex = loadTexture(fullPath);
                if (specularTex && specularTex->ID != 0) {
                    material->specularMap = specularTex;
                } else {
                    material->specularMap = nullptr;
                }
            } else {
                material->specularMap = nullptr;
            }
        }
        
        // Load material properties
        aiColor3D diffuseColor;
        if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
        {
            material->diffuseColor = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
        }
        
        float shininess;
        if (aiMat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
        {
            material->shininess = shininess;
        }
        
        materials.push_back(material);
    }
    else
    {
        std::cout << "No material assigned to mesh" << std::endl;
        // Create a default material if no material is assigned
        Material* defaultMaterial = new Material();
        defaultMaterial->materialIndex = 1 + (globalMaterialIndexCounter % 15); // Keep within 1-15 range
        globalMaterialIndexCounter++;
        defaultMaterial->shader = modelShader; // Assign the model's shader
        defaultMaterial->diffuseColor = glm::vec3(0.8f, 0.8f, 0.8f);
        defaultMaterial->shininess = 32.0f;
        materials.push_back(defaultMaterial);
    }

    Mesh result = Mesh(vertices, indices, materials);
    return result;
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

void Model::updateTransform() const
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

glm::mat4 Model::getTransform() const {
    if (dirty)
        updateTransform();
    return cachedTransform;
}
