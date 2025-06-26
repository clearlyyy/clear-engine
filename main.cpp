//main.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Shader.h"
#include "Texture.h"
#include "PointLight.h"
#include "Primitives.h"
#include "Renderer.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "GUI.h"
#include "Skybox.h"

#include "Globals.h"
#include "GBuffer.h"

#include <vector>
#include <random>
#include "SSAO.h"
#include "ShadowMap.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
bool checkForShaderProgramLinkErrors(unsigned int program);



bool wireframe = false;

Camera cam(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;
bool shouldProcessMouse = true;
bool isSprinting = false;
bool flashLightEnabled = true;
bool visualizeNormals = false;
bool visualizeSSAO = false;
bool visualizeShadowMap = false;

std::vector<Light*> lights;

int main()
{

    // GLFW Init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Clear Engine", NULL, NULL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwMakeContextCurrent(window);
    // GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set GL Viewport Size
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);

    std::cout << "Starting..." << std::endl;

    //Shader
    Shader basicShader("vertexShader.glsl", "fragmentShader.glsl");
    Shader lightShader("lightcube.vs", "lightcube.fs");

    //Load Texture
    Texture boxTex("container2.png", true);
    Texture boxSpec("container2_specular.png", true);

    Material basicMaterial;
    basicMaterial.shader = &basicShader;
    basicMaterial.diffuseMap = &boxTex;
    basicMaterial.specularMap = &boxSpec;
    basicMaterial.shininess = 64;
    basicMaterial.hasTransparency = true;

    Model cubeModel;
    Cube cubeMesh(&basicMaterial);
    cubeModel.addMesh(cubeMesh);
    cubeModel.setName("Cube");

    std::vector<Model*> models;
    models.push_back(&cubeModel);

    Model sponza;
    sponza.loadModel("Sponza/sponza.obj", &basicShader);
    models.push_back(&sponza);
    sponza.setScale(glm::vec3(0.05f, 0.05f, 0.05f));
    sponza.setPosition(glm::vec3(0.0f, -10.0f, 0.0f));
    sponza.setName("Sponza");
    
    Model room;
    room.loadModel("Room/Room-A.obj", &basicShader);
    models.push_back(&room);
    room.setScale(glm::vec3(0.05f, 0.05f, 0.05f));
    room.setPosition(glm::vec3(0.0f, -10.0f, 0.0f));
    room.setName("Living Room");

    Model jet;
    jet.loadModel("Jet/Formula 1 mesh.obj", &basicShader);
    models.push_back(&jet);
    jet.setScale(glm::vec3(0.05f, 0.05f, 0.05f));
    jet.setPosition(glm::vec3(0.0f, -10.0f, 0.0f));
    jet.setName("Jet");

    Renderer renderer;
    
    PointLight* light = new PointLight(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.4f, 1.0f, 4.0f));
    light->setFalloff(1.0f, 0.09f, 0.032f);
    light->applyToShader(basicShader, 0);
    models.push_back(&light->getLightModel());
    lights.push_back(light);

    PointLight* light2 = new PointLight(glm::vec3(-2.0f, -1.0f, -2.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.5f, 0.8f, 0.5f), glm::vec3(1.0f, 0.4f, 0.4f));
    light2->setFalloff(1.0f, 0.09f, 0.032f);
    light2->applyToShader(basicShader, 1);
    models.push_back(&light2->getLightModel());
    lights.push_back(light2);

    DirectionalLight* dirLight = new DirectionalLight(
        glm::vec3(0.1f, -1.0f, 0.1f), // Direction
        glm::vec3(0.4f, 0.4f, 0.4f),    // Ambient
        glm::vec3(0.7f, 0.7f, 0.7f),    // Diffuse
        glm::vec3(1.0f, 1.0f, 1.0f)     // Specular
    );
    dirLight->applyToShader(basicShader, 2);
    models.push_back(&dirLight->getLightModel());
    lights.push_back(dirLight);

    SpotLight* flashLight = new SpotLight(glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), cam.getPosition(), cam.Front, glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.5f, 0.8f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f));
    flashLight->applyToShader(basicShader, 3);
    models.push_back(&flashLight->getLightModel());
    lights.push_back(flashLight);

    // Set the total number of lights in the shader
    basicShader.use();
    basicShader.setInt("numLights", lights.size());

    // SSAO setup
    SSAO ssao(WINDOW_WIDTH, WINDOW_HEIGHT);

    // GUI
    GUI mainGUI(window);
    mainGUI.setSSAO(&ssao);
    cubeModel.setPosition(glm::vec3(2.0f, 0.0f, 0.0f));

    glfwSwapInterval( 0 );

    // G-buffer FBO setup for deferred shading
    GBuffer gbuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Create geometry pass shader once
    Shader geometryPass("gBuffer.vs", "gBuffer.fs");

    //Shadow Map setup
    ShadowMap shadowMap(2048, 2048);
    Shader shadowShader("shadow_depth.vs", "shadow_depth.fs");

    // Create lighting pass shader
    Shader deferredLightingShader("post.vs", "deferred_lighting.fs");

    // Create SSAO shader
    Shader ssaoShader("ssao.vs", "ssao.fs");
    Shader ssaoBlurShader("ssao_blur.vs", "ssao_blur.fs");

    // Create a simple SSAO visualization shader
    Shader ssaoVisualizeShader("post.vs", "ssao_visualize.fs");

    // Add shadow map debug shader
    Shader shadowMapDebugShader("post.vs", "shadow_map_debug.fs");

    // Add shadow blur shader
    Shader shadowBlurShader("shadow_blur.vs", "shadow_blur.fs");

    //Quad to render RBO Onto
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Configure shadow map for directional light
    shadowMap.setupForDirectionalLight(dirLight->Direction, glm::vec3(0.0f), 100.0f);
    
    //Skybox
    Skybox sky("skybox");

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Input
        processInput(window);

        // Update shadow map configuration for directional light
        shadowMap.setupForDirectionalLight(dirLight->Direction, glm::vec3(0.0f), 20.0f);

        // Shadow pass
        shadowMap.beginShadowPass();
        shadowMap.renderModels(models, shadowShader);
        shadowMap.endShadowPass();

        // Shadow blur pass
        shadowMap.blurShadowMap(shadowBlurShader, quadVAO);

        glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);

        // Set wireframe mode
        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (wireframe) {
            // Wireframe mode: Use forward rendering directly to screen
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            mainGUI.BeginFrame();
            
            // Draw skybox first
            glDepthFunc(GL_LEQUAL);
            sky.Draw(cam);
            glDepthFunc(GL_LESS);
            
            // Render all models (including light models) with forward rendering
            renderer.render(models, cam);
        } else {
            // Normal deferred rendering pipeline
            gbuffer.bindForWriting();
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            mainGUI.BeginFrame();
            glClearBufferfv(GL_COLOR, 2, glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)));
            
            renderer.render(models, cam, &geometryPass);
            gbuffer.unbind();
            glDisable(GL_DEPTH_TEST);
            // Clear the default framebuffer before drawing the quad
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Draw skybox first to the default framebuffer
            glEnable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT);
            glDepthFunc(GL_LEQUAL);
            sky.Draw(cam);
            glDepthFunc(GL_LESS);
            glDisable(GL_DEPTH_TEST);

            // SSAO pass
            ssao.renderSSAO(gbuffer, cam.getProjectionMatrix(WINDOW_WIDTH / WINDOW_HEIGHT), ssaoShader, quadVAO);

            // SSAO blur pass
            ssao.blurSSAO(ssaoBlurShader, quadVAO);
            glm::mat4 viewMatrix = cam.GetViewMatrix();
            // 2. Lighting/Visualize Pass: render quad with selected buffer
            glBindVertexArray(quadVAO);
            glActiveTexture(GL_TEXTURE0);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            if (visualizeNormals) {
                Shader visualizeNormalsShader("post.vs", "visualize_normals.fs");
                visualizeNormalsShader.use();
                visualizeNormalsShader.setInt("gNormal", 0);
                glBindTexture(GL_TEXTURE_2D, gbuffer.getNormalTexture());
            } else if (visualizeSSAO) {
                ssaoVisualizeShader.use();
                ssaoVisualizeShader.setInt("ssaoTex", 0);
                glBindTexture(GL_TEXTURE_2D, ssao.getSSAOTexture());
            } else if (visualizeShadowMap) {
                shadowMapDebugShader.use();
                shadowMapDebugShader.setInt("shadowMap", 0);
                shadowMap.bindBlurredDepthMap(0);
            } else {
                deferredLightingShader.use();
                deferredLightingShader.setInt("gPosition", 0);
                deferredLightingShader.setInt("gPositionWorld", 1);
                deferredLightingShader.setInt("gNormal", 2);
                deferredLightingShader.setInt("gAlbedo", 3);
                deferredLightingShader.setInt("ssao", 4);
                deferredLightingShader.setInt("shadowMap", 5);
                deferredLightingShader.setInt("gSpecular", 6);
                deferredLightingShader.setMat4("lightSpaceMatrix", shadowMap.getLightSpaceMatrix());
                
                // Bind G-buffer textures
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, gbuffer.getPositionTexture());
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, gbuffer.getWorldPositionTexture());
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, gbuffer.getNormalTexture());
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, gbuffer.getAlbedoTexture());
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, ssao.getSSAOTexture());
                glActiveTexture(GL_TEXTURE5);
                shadowMap.bindBlurredDepthMap(5);
                glActiveTexture(GL_TEXTURE6);
                glBindTexture(GL_TEXTURE_2D, gbuffer.getSpecularTexture());
                // Set camera position
                deferredLightingShader.setVec3("viewPos", cam.getPosition());
                // Set all lights by type
                int pointLightCount = 0;
                int dirLightCount = 0;
                int spotLightCount = 0;
                
                for (Light* l : lights) {
                    if (l->isEnabled()) {
                    switch (l->getType()) {
                        case TYPE_POINT: {
                            // Transform position to view space
                            glm::vec3 viewSpacePos = glm::vec3(viewMatrix * glm::vec4(l->Position, 1.0f));
                            l->setDeferredUniforms(deferredLightingShader, pointLightCount++, &viewSpacePos);
                            break;
                        }
                        case TYPE_DIRECTIONAL:
                            l->setDeferredUniforms(deferredLightingShader, dirLightCount++);
                            break;
                        case TYPE_SPOT: {
                            if (l->state) {
                                // Transform position and direction to view space
                                glm::vec3 viewSpacePos = glm::vec3(viewMatrix * glm::vec4(l->Position, 1.0f));
                                glm::vec3 viewSpaceDir = glm::mat3(viewMatrix) * static_cast<SpotLight*>(l)->Direction;
                                l->setDeferredUniforms(deferredLightingShader, spotLightCount++, &viewSpacePos, &viewSpaceDir);
                            }
                            break;
                        }
                    }
                    }
                }
                deferredLightingShader.setInt("numPointLights", pointLightCount);
                deferredLightingShader.setInt("numDirLights", dirLightCount);
                deferredLightingShader.setInt("numSpotLights", spotLightCount);
            }
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glDisable(GL_BLEND);
        }
        
        mainGUI.RenderGUI("clear-engine", models, lights);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    mainGUI.killGUI();
    glfwTerminate();

    // At the end of main, free the lights
    for (Light* l : lights) delete l;

    return 0;
}


// Resize the GL Viewport when we resize the window.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        isSprinting = true;
    else
        isSprinting = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.processKeyboard(FORWARD, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.processKeyboard(BACKWARD, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.processKeyboard(LEFT, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.processKeyboard(RIGHT, deltaTime, isSprinting);
    
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        wireframe = !wireframe;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        shouldProcessMouse = !shouldProcessMouse;
        if (shouldProcessMouse) { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
        else { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        flashLightEnabled = !flashLightEnabled;
        // Set the state of the spotlight (flashLight)
        // Find the first SpotLight in the lights vector and toggle its state
        for (Light* l : lights) {
            if (l->getType() == TYPE_SPOT) {
                l->state = flashLightEnabled;
                break;
            }
        }
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS)
        visualizeNormals = !visualizeNormals;
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        visualizeSSAO = !visualizeSSAO;
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
        visualizeShadowMap = !visualizeShadowMap;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{   
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;


    if (shouldProcessMouse) {
        cam.processMouseMovement(xoffset, yoffset);
    }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.processMouseScroll(static_cast<float>(yoffset));
}
