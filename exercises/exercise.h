#include <GLFW/glfw3.h>

#define DELCARE_EXERCISE(N) TMP_DELCARE_EXERCISE(N)
#define TMP_DELCARE_EXERCISE(N) void exercise##N(GLFWwindow* window)

#define EXERCISE(N, W)  TMP_EXERCISE(N, W)
#define TMP_EXERCISE(N, W)  exercise##N(W)
