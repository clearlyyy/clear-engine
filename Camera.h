#include <glm/glm.hpp>

#include "Shader.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
    private:

        //Camera Transforms
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        
        float Yaw;
        float Pitch;

        float cameraSpeed;
        float mouseSens;
        float Zoom;

        void updateCameraVectors();
        
        public:
        Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
        void processKeyboard(Camera_Movement direction, float deltaTime);
        void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
        void processMouseScroll(float yoffset);
        glm::mat4 GetViewMatrix();
        glm::vec3 getPosition();
};