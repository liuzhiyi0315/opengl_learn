cmake_minimum_required(VERSION 3.0.0)
project(opengl_learn VERSION 0.1.0)

find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

include(CTest)
enable_testing()

link_libraries(glfw ${GLEW_LIBRARIES})
add_executable(opengl_learn main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
