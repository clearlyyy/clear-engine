#pragma once

#include <vector>

#include "Model.h"
#include "Camera.h"
#include "Globals.h"
#include "Shader.h"


// Renderer
// This should take in a list of models, the camera and render stuff.

class Renderer {
    public:
        Renderer();

        void render(std::vector<Model*> &models, Camera &camera);
        void render(std::vector<Model*> &models, Camera &camera, Shader* geometryShader);

        

};