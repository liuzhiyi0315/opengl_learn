cmake_minimum_required(VERSION 3.0.0)
project(opengl_learn VERSION 0.1.0)

find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

include(CTest)
enable_testing()

link_libraries(glfw ${GLEW_LIBRARIES} GL assimp)

file(GLOB SRC_UTILS utils/*.*)
file(GLOB SRC_EXERCISES exercises/exercise20.cpp)

add_executable(opengl_learn main.cpp ${SRC_UTILS} ${SRC_EXERCISES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
