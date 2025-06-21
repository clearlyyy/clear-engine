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

#include "Globals.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
bool checkForShaderProgramLinkErrors(unsigned int program);



bool wireframe = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera cam(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

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

    Model cubeModel;
    Cube cubeMesh(&basicMaterial);
    cubeModel.addMesh(cubeMesh);

    std::vector<Model*> models;
    models.push_back(&cubeModel);

    Renderer renderer;

    PointLight light(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.4f, 1.0f, 4.0f));
    light.applyToShader(basicShader, 0);
    models.push_back(&light.getLightModel());

    PointLight light2(glm::vec3(-2.0f, -1.0f, -2.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.5f, 0.8f, 0.5f), glm::vec3(1.0f, 0.4f, 0.4f));
    light2.applyToShader(basicShader, 1);
    models.push_back(&light2.getLightModel());

    glm::vec3 cubePositions[] = {
       glm::vec3( 0.0f,  0.0f,  0.0f), 
       glm::vec3( 2.0f,  5.0f, -15.0f), 
       glm::vec3(-1.5f, -2.2f, -2.5f),  
       glm::vec3(-3.8f, -2.0f, -12.3f),  
       glm::vec3( 2.4f, -0.4f, -3.5f),  
       glm::vec3(-1.7f,  3.0f, -7.5f),  
       glm::vec3( 1.3f, -2.0f, -2.5f),  
       glm::vec3( 1.5f,  2.0f, -2.5f), 
       glm::vec3( 1.5f,  0.2f, -1.5f), 
       glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Input
        processInput(window);

        //Rendering
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        basicShader.use();
        light.applyToShader(basicShader, 0);
        light2.applyToShader(basicShader, 1);
        
        for (unsigned int i = 0; i < 10; i++)
        {
            float angle = 20.0f * i + 3;
            float rot = (float)glfwGetTime() * glm::radians(angle*4);
            cubeModel.setPosition(cubePositions[i]);
            cubeModel.setRotation(glm::vec3(rot,rot,rot));
            renderer.render(models, cam);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


// Resize the GL Viewport when we resize the window.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.processKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        wireframe = !wireframe;

    
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

    cam.processMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.processMouseScroll(static_cast<float>(yoffset));
}
