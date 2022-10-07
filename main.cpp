#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdio.h>
#include "utils/shader_parse.h"
#include "exercises/exercise.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <unistd.h>
#define EXEC_SEQ 19

DELCARE_EXERCISE(EXEC_SEQ);

bool is_glfw_key_pressed(GLFWwindow* window, int key) {
    static double lastPress = glfwGetTime();
    auto now = glfwGetTime();

    if (glfwGetKey(window, key) == GLFW_PRESS) {
        if ((now - lastPress) > 0.5) {
            lastPress = now;
            return true;
        }
    }

    return false;
}

double get_frame_delta() {
    static double lastFrame = glfwGetTime();
    auto currentFrame = glfwGetTime();

    lastFrame = currentFrame;

    auto deltaTime = currentFrame - lastFrame;

    return deltaTime > 0?deltaTime:1;
}

void processCameraInput(GLFWwindow *window, glm::vec3& cameraPos)
{
    auto deltaTime = get_frame_delta();
    float cameraSpeed = 2.0f * deltaTime; // adjust accordingly

    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    if (is_glfw_key_pressed(window, GLFW_KEY_W)) {
        cameraPos += cameraSpeed * cameraFront;
    }

    if (is_glfw_key_pressed(window, GLFW_KEY_S)) {
        cameraPos -= cameraSpeed * cameraFront;
    }

    if (is_glfw_key_pressed(window, GLFW_KEY_A)) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    
    if (is_glfw_key_pressed(window, GLFW_KEY_D)) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
}

int main() {
    // read shader files
    // char  buff[256];
    // std::string cwd = getcwd(buff, 256);

    // create window by GLFW
    if (!glfwInit()) return -1;
    auto window = glfwCreateWindow(1024, 768, "Hello GL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        glfwTerminate();
        return -1;
    }
    
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    EXERCISE(EXEC_SEQ, window);
    
    glfwTerminate();
    return 0;
}