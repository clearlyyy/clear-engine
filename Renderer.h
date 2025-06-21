#include <vector>

#include "Model.h"
#include "Camera.h"
#include "Globals.h"


// Renderer
// This should take in a list of models, the camera and render stuff.

class Renderer {
    public:
        Renderer();

        void render(std::vector<Model*> &models, Camera &camera);

        

};