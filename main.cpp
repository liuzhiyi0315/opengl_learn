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
#define EXEC_SEQ 21

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

    glfwWindowHint(GLFW_SAMPLES, 4);

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

void RotateArbitraryLine(glm::mat4& rotateMat, glm::vec3 v1, glm::vec3 v2, float theta)
{
    float a = v1.x;
    float b = v1.y;
    float c = v1.z;

    glm::vec3 p = glm::normalize(v2 - v1);
    float u = p.x;
    float v = p.y;
    float w = p.z;

    float uu = u * u;
    float uv = u * v;
    float uw = u * w;
    float vv = v * v;
    float vw = v * w;
    float ww = w * w;
    float au = a * u;
    float av = a * v;
    float aw = a * w;
    float bu = b * u;
    float bv = b * v;
    float bw = b * w;
    float cu = c * u;
    float cv = c * v;
    float cw = c * w;

    float costheta = cosf(theta);
    float sintheta = sinf(theta);

    rotateMat[0].x = uu + (vv + ww) * costheta;
    rotateMat[0].y = uv * (1 - costheta) + w * sintheta;
    rotateMat[0].z = uw * (1 - costheta) - v * sintheta;
    rotateMat[0].w = 0;

    rotateMat[1].x = uv * (1 - costheta) - w * sintheta;
    rotateMat[1].y = vv + (uu + ww) * costheta;
    rotateMat[1].z = vw * (1 - costheta) + u * sintheta;
    rotateMat[1].w = 0;

    rotateMat[2].x = uw * (1 - costheta) + v * sintheta;
    rotateMat[2].y = vw * (1 - costheta) - u * sintheta;
    rotateMat[2].z = ww + (uu + vv) * costheta;
    rotateMat[2].w = 0;

    rotateMat[3].x = (a * (vv + ww) - u * (bv + cw)) * (1 - costheta) + (bw - cv) * sintheta;
    rotateMat[3].y = (b * (uu + ww) - v * (au + cw)) * (1 - costheta) + (cu - aw) * sintheta;
    rotateMat[3].z = (c * (uu + vv) - w * (au + bv)) * (1 - costheta) + (av - bu) * sintheta;
    rotateMat[3].w = 1;
}

glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
        return;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    static float lastX = xpos;
    static float lastY = ypos;

    static float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    static float pitch =  0.0f;

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
}