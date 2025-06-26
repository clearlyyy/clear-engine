#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Globals.h"
#include <string>
#include <vector>
#include "Model.h"
#include "SSAO.h"
#include "Light.h"

class GUI {
    public:
        GUI(GLFWwindow* window);
        void setSSAO(SSAO* ssaoPtr) { ssao = ssaoPtr; }
        void setLights(std::vector<Light*>* lightsPtr) { lights = lightsPtr; }

        void BeginFrame();

        void RenderGUI(std::string windowName, const std::vector<Model*>& models, const std::vector<Light*>& lights);

        void killGUI();

        // Draw call tracking
        static void IncrementDrawCalls() { drawCallsThisFrame++; }
        static void ResetDrawCalls() { drawCallsThisFrame = 0; }
        static int GetDrawCalls() { return drawCallsThisFrame; }

    private:
        static int drawCallsThisFrame;
        SSAO* ssao = nullptr;
        std::vector<Light*>* lights = nullptr;
};
