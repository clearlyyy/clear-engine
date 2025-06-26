#include "GUI.h"

// Define static member variable
int GUI::drawCallsThisFrame = 0;

GUI::GUI(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 440");
}

void GUI::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::RenderGUI(std::string windowName, const std::vector<Model*>& models, const std::vector<Light*>& lights)
{
    using namespace ImGui;

    Begin(windowName.c_str());
    float fps = 1.0f / std::max(deltaTime, 0.00001f);
    std::string fpsText = "FPS: " + std::to_string(fps);
    Text(fpsText.c_str());

    // Display draw calls
    std::string drawCallsText = "Draw Calls: " + std::to_string(GetDrawCalls());
    Text(drawCallsText.c_str());

    Separator();
    Text("Models:");
    for (auto* model : models) {
        if (model) {
            if (CollapsingHeader(model->name.c_str())) {
                if (ImGui::SliderFloat3((model->name + "Position").c_str(), &model->position.x, -100.0f, 100.0f)) {
                    model->setDirty();
                }
                if (ImGui::SliderFloat3((model->name + "rotation").c_str(), &model->rotation.x, 0.0f, 360.0f)) {
                    model->setDirty();
                }
            }
        }
    }

    // SSAO Controls
    if (ssao) {
        Separator();
        Text("SSAO Settings:");
        ImGui::SliderFloat("SSAO Radius", &ssao->radius, 0.01f, 20.0f);
        ImGui::SliderFloat("SSAO Bias", &ssao->bias, 0.0f, 1.0f);
        ImGui::SliderFloat("SSAO Strength", &ssao->strength, 0.0f, 8.0f);
    }

    // Light toggles
    if (!lights.empty()) {
        Separator();
        Text("Lights:");
        // Type toggles
        static bool showPoint = true, showDir = true, showSpot = true;
        Checkbox("Enable Point Lights", &showPoint);
        Checkbox("Enable Directional Lights", &showDir);
        Checkbox("Enable Spot Lights", &showSpot);
        for (size_t i = 0; i < lights.size(); ++i) {
            Light* l = lights[i];
            int type = l->getType();
            bool show = (type == TYPE_POINT && showPoint) || (type == TYPE_DIRECTIONAL && showDir) || (type == TYPE_SPOT && showSpot);
            if (show) {
                std::string label = l->name.empty() ? ("Light " + std::to_string(i)) : l->name;
                Checkbox(("Enable##" + std::to_string(i)).c_str(), &l->enabled);
                SameLine();
                Text("%s", label.c_str());
            } else {
                l->enabled = false;
            }
        }
    }

    End();
    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
    
    // Reset draw call counter for next frame
    ResetDrawCalls();
}

void GUI::killGUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}