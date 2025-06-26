#include <vector>
#include "stb_image.h"
#include <string>
#include <iostream>
#include <glad/glad.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include "Camera.h"
#include "Globals.h"

class Skybox {
    private:
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int cubeMapTexture;
    Shader skyboxShader;
    std::vector<std::string> faces;
    unsigned int loadCubemap();
    void createMesh();

    public:
    Skybox(std::string location);
    void Draw(Camera cam);
    unsigned int getCubemapTexture() const { return cubeMapTexture; }

};