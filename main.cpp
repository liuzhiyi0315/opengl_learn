#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdio.h>
#include "utils/shader_parse.h"
#include "exercises/exercise.h"
// #include <unistd.h>
#define EXEC_SEQ 5

DELCARE_EXERCISE(EXEC_SEQ);

int main() {
    // read shader files
    // char  buff[256];
    // std::string cwd = getcwd(buff, 256);

    // create window by GLFW
    if (!glfwInit()) return -1;
    auto window = glfwCreateWindow(640, 480, "Hello GL", NULL, NULL);
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