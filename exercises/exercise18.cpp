#include "../utils/shader_parse.h"
#include "../utils/texture_parse.h"
#include "../utils/material_parse.h"
#include "../utils/model.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

bool is_glfw_key_pressed(GLFWwindow* window, int key);

double get_frame_delta();

void processCameraInput(GLFWwindow *window, glm::vec3& cameraPos);

static glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
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

void exercise18(GLFWwindow* window) {
    struct ShaderInfo
    {
        unsigned int id;
        unsigned int uniformBlockIndex;
        unsigned int modelLoc;
        unsigned int viewDirLoc;
        float timeLoc;
    };

    // object shader program
    // shared vertex source
    auto vertexShaderSource = ParseShader("../res/shader/advanced/e18/e18_vertex.shader");

    std::string frag_name[4] = {"red", "green", "blue", "yellow"};

    ShaderInfo color4_shaders[5];
    char fragmentSourcePath[64];

    for (auto i = 0; i < 4; i ++ ) {
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);

        std::sprintf(fragmentSourcePath, "../res/shader/advanced/e18/e18_fragment_%s.shader", frag_name[i].c_str());
        auto fragmentShaderSource = ParseShader(fragmentSourcePath);

        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        unsigned int shaders[] = {vertexShader, fragmentShader};
        color4_shaders[i].id = createShaderProgram(shaders);
        color4_shaders[i].uniformBlockIndex = glGetUniformBlockIndex(color4_shaders[i].id, "Matrices");
        glUniformBlockBinding(color4_shaders[i].id, color4_shaders[i].uniformBlockIndex, 0);
        color4_shaders[i].modelLoc = glGetUniformLocation(color4_shaders[i].id, "model");
    }

    {
        const int i = 4;
        auto vertexGeoShaderSource = ParseShader("../res/shader/advanced/e18/e18_vertex_geo.shader");
        auto vertexGeoShader = createShader(GL_VERTEX_SHADER, vertexGeoShaderSource);

        auto geometryShaderSource = ParseShader("../res/shader/advanced/e18/e18_geometry.shader");
        auto geometryShader = createShader(GL_GEOMETRY_SHADER, geometryShaderSource);

        std::sprintf(fragmentSourcePath, "../res/shader/advanced/e18/e18_fragment_%s.shader", "normal");
        auto fragmentShaderSource = ParseShader(fragmentSourcePath);
        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

        unsigned int geo_shaders[] = {vertexGeoShader, geometryShader, fragmentShader};
        color4_shaders[i].id = createShaderProgram(geo_shaders, 3);
        color4_shaders[i].uniformBlockIndex = glGetUniformBlockIndex(color4_shaders[i].id, "Matrices");
        glUniformBlockBinding(color4_shaders[i].id, color4_shaders[i].uniformBlockIndex, 0);
        color4_shaders[i].modelLoc = glGetUniformLocation(color4_shaders[i].id, "model");
    }

    ShaderInfo nanosuitShader;

    {
        auto vertexShaderSource = ParseShader("../res/shader/advanced/e18/e18_vertex_nanosuit.shader");
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);

        auto geometryShaderSource = ParseShader("../res/shader/advanced/e18/e18_geometry_nanosuit.shader");
        auto geometryShader = createShader(GL_GEOMETRY_SHADER, geometryShaderSource);

        auto fragmentShaderSource = ParseShader("../res/shader/advanced/e18/e18_fragment_nanosuit.shader");
        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        
        unsigned int shaders[] = {vertexShader, geometryShader, fragmentShader};
        nanosuitShader.id = createShaderProgram(shaders, 3);
        nanosuitShader.uniformBlockIndex = glGetUniformBlockIndex(nanosuitShader.id, "Matrices");
        glUniformBlockBinding(nanosuitShader.id, nanosuitShader.uniformBlockIndex, 0);
        nanosuitShader.modelLoc = glGetUniformLocation(nanosuitShader.id, "model");
        nanosuitShader.viewDirLoc = glGetUniformLocation(nanosuitShader.id, "viewDir");
        nanosuitShader.timeLoc = glGetUniformLocation(nanosuitShader.id, "time");
    }


    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    // 投影矩阵
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    float cube_vertices[] {
        /*第一个面*/
        /*坐标1*/ 0.5, -0.5,  0.5, /*颜色*/1.0f, 0.0f, 0.0f, /*纹理坐标1*/0.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/0.0f, 0.0f, 1.0f,
        /*坐标2*/-0.5, -0.5,  0.5, /*颜色*/1.0f, 0.0f, 0.0f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/1.0f, 0.0f , /*法线坐标*/0.0f, 0.0f, 1.0f,
        /*坐标3*/-0.5,  0.5,  0.5, /*颜色*/1.0f, 0.0f, 0.0f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/0.0f, 0.0f, 1.0f,
        /*坐标3*/-0.5,  0.5,  0.5, /*颜色*/1.0f, 0.0f, 0.0f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/0.0f, 0.0f, 1.0f,
        /*坐标4*/ 0.5,  0.5,  0.5, /*颜色*/1.0f, 0.0f, 0.0f, /*纹理坐标1*/0.0f, 1.0f, /*纹理坐标2*/0.0f, 1.0f , /*法线坐标*/0.0f, 0.0f, 1.0f,
        /*坐标1*/ 0.5, -0.5,  0.5, /*颜色*/1.0f, 0.0f, 0.0f, /*纹理坐标1*/0.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/0.0f, 0.0f, 1.0f,

        /*第二个面*/
        /*坐标1*/-0.5, -0.5,  0.5, /*颜色*/1.0f, 0.5f, 0.0f, /*纹理坐标1*/0.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/-1.0f, 0.0f, 0.0f,
        /*坐标2*/-0.5, -0.5, -0.5, /*颜色*/1.0f, 0.5f, 0.0f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/1.0f, 0.0f , /*法线坐标*/-1.0f, 0.0f, 0.0f,
        /*坐标3*/-0.5,  0.5, -0.5, /*颜色*/1.0f, 0.5f, 0.0f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/-1.0f, 0.0f, 0.0f,
        /*坐标3*/-0.5,  0.5, -0.5, /*颜色*/1.0f, 0.5f, 0.0f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/-1.0f, 0.0f, 0.0f,
        /*坐标4*/-0.5,  0.5,  0.5, /*颜色*/1.0f, 0.5f, 0.0f, /*纹理坐标1*/0.0f, 1.0f, /*纹理坐标2*/0.0f, 1.0f , /*法线坐标*/-1.0f, 0.0f, 0.0f,
        /*坐标1*/-0.5, -0.5,  0.5, /*颜色*/1.0f, 0.5f, 0.0f, /*纹理坐标1*/0.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/-1.0f, 0.0f, 0.0f,

        /*第三个面*/
        /*坐标1*/ 0.5,  0.5,  0.5, /*颜色*/1.0f, 1.0f, 0.0f, /*纹理坐标1*/0.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/0.0f,  1.0f, 0.0f,
        /*坐标2*/-0.5,  0.5,  0.5, /*颜色*/1.0f, 1.0f, 0.0f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/1.0f, 0.0f , /*法线坐标*/0.0f,  1.0f, 0.0f,
        /*坐标3*/-0.5,  0.5, -0.5, /*颜色*/1.0f, 1.0f, 0.0f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/0.0f,  1.0f, 0.0f,
        /*坐标3*/-0.5,  0.5, -0.5, /*颜色*/1.0f, 1.0f, 0.0f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/0.0f,  1.0f, 0.0f,
        /*坐标4*/ 0.5,  0.5, -0.5, /*颜色*/1.0f, 1.0f, 0.0f, /*纹理坐标1*/0.0f, 1.0f, /*纹理坐标2*/0.0f, 1.0f , /*法线坐标*/0.0f,  1.0f, 0.0f,
        /*坐标1*/ 0.5,  0.5,  0.5, /*颜色*/1.0f, 1.0f, 0.0f, /*纹理坐标1*/0.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/0.0f,  1.0f, 0.0f,

        /*第四个面*/
        /*坐标1*/ 0.5,  0.5,  0.5, /*颜色*/0.0f, 1.0f, 0.0f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/ 1.0f, 0.0f, 0.0f,
        /*坐标2*/ 0.5,  0.5, -0.5, /*颜色*/0.0f, 1.0f, 0.0f, /*纹理坐标1*/0.0f, 1.0f, /*纹理坐标2*/0.0f, 1.0f , /*法线坐标*/ 1.0f, 0.0f, 0.0f,
        /*坐标3*/ 0.5, -0.5, -0.5, /*颜色*/0.0f, 1.0f, 0.0f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/ 1.0f, 0.0f, 0.0f,
        /*坐标3*/ 0.5, -0.5, -0.5, /*颜色*/0.0f, 1.0f, 0.0f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/ 1.0f, 0.0f, 0.0f,
        /*坐标4*/ 0.5, -0.5,  0.5, /*颜色*/0.0f, 1.0f, 0.0f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/1.0f, 0.0f , /*法线坐标*/ 1.0f, 0.0f, 0.0f,
        /*坐标1*/ 0.5,  0.5,  0.5, /*颜色*/0.0f, 1.0f, 0.0f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/ 1.0f, 0.0f, 0.0f,

        /*第五个面*/
        /*坐标1*/ 0.5,  0.5, -0.5, /*颜色*/0.0f, 1.0f, 0.5f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/0.0f, 0.0f, -1.0f,
        /*坐标2*/-0.5,  0.5, -0.5, /*颜色*/0.0f, 1.0f, 0.5f, /*纹理坐标1*/0.0f, 1.0f, /*纹理坐标2*/0.0f, 1.0f , /*法线坐标*/0.0f, 0.0f, -1.0f,
        /*坐标3*/-0.5, -0.5, -0.5, /*颜色*/0.0f, 1.0f, 0.5f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/0.0f, 0.0f, -1.0f,
        /*坐标3*/-0.5, -0.5, -0.5, /*颜色*/0.0f, 1.0f, 0.5f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/0.0f, 0.0f, -1.0f,
        /*坐标4*/ 0.5, -0.5, -0.5, /*颜色*/0.0f, 1.0f, 0.5f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/1.0f, 0.0f , /*法线坐标*/0.0f, 0.0f, -1.0f,
        /*坐标1*/ 0.5,  0.5, -0.5, /*颜色*/0.0f, 1.0f, 0.5f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/0.0f, 0.0f, -1.0f,

        /*第六个面*/
        /*坐标1*/ 0.5, -0.5, -0.5, /*颜色*/0.0f, 0.0f, 0.5f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/0.0f, -1.0f, 0.0f,
        /*坐标2*/-0.5, -0.5, -0.5, /*颜色*/0.0f, 0.0f, 0.5f, /*纹理坐标1*/0.0f, 1.0f, /*纹理坐标2*/0.0f, 1.0f , /*法线坐标*/0.0f, -1.0f, 0.0f,
        /*坐标3*/-0.5, -0.5,  0.5, /*颜色*/0.0f, 0.0f, 0.5f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/0.0f, -1.0f, 0.0f,
        /*坐标3*/-0.5, -0.5,  0.5, /*颜色*/0.0f, 0.0f, 0.5f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/0.0f, 0.0f , /*法线坐标*/0.0f, -1.0f, 0.0f,
        /*坐标4*/ 0.5, -0.5,  0.5, /*颜色*/0.0f, 0.0f, 0.5f, /*纹理坐标1*/1.0f, 0.0f, /*纹理坐标2*/1.0f, 0.0f , /*法线坐标*/0.0f, -1.0f, 0.0f,
        /*坐标1*/ 0.5, -0.5, -0.5, /*颜色*/0.0f, 0.0f, 0.5f, /*纹理坐标1*/1.0f, 1.0f, /*纹理坐标2*/1.0f, 1.0f , /*法线坐标*/0.0f, -1.0f, 0.0f
    };


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    int strider_units = 13;

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strider_units * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strider_units * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // 纹理属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, strider_units * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // 纹理属性2
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, strider_units * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // 法线属性
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, strider_units * sizeof(float), (void*)(10 * sizeof(float)));
    glEnableVertexAttribArray(4);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    glm::vec3 cube_postions[4] = {
        glm::vec3( 0.0,  0.0, 0.0),
        glm::vec3(-1.0,  2.0, 0.0),
        glm::vec3( 1.0, -2.0, 0.0),
        glm::vec3(-1.0, -2.0, 0.0)
    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
   
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

    Model nanosuit;
    nanosuit.loadModel("../res/models/nanosuit/nanosuit.obj");

    ////////////////////////////////////////////////////////////////////
    glfwSetCursorPosCallback(window, mouse_callback);

    while (! glfwWindowShouldClose(window))
	{       
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_END) == GLFW_PRESS) {
             glfwSetWindowShouldClose(window, true);
        }
    
        // render
        // ------
        
        glClearColor(0.25f, 0.35f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // 视图矩阵
        processCameraInput(window, cameraPos);
        cameraFront = glm::normalize(front);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 

        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindVertexArray(VAO);

        glm::mat4 cube_model = glm::mat4(1.0f);
        
        for (int i = 0; i < 5; i ++) {
            glUseProgram(color4_shaders[i].id);
            if (i < 4) {
                cube_model = glm::translate(cube_model, cube_postions[i]);
                cube_model = glm::rotate(cube_model, (float)glfwGetTime() * glm::radians(-5.0f), glm::vec3(0.5f, 1.0f, 0.0f));
            }
            glUniformMatrix4fv(color4_shaders[i].modelLoc, 1, GL_FALSE, glm::value_ptr(cube_model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glUseProgram(nanosuitShader.id);
        glm::mat4 suit_model = glm::mat4(1.0f);
        suit_model = glm::translate(suit_model, glm::vec3(3, -3.8, 0.0));
        suit_model = glm::scale(suit_model, glm::vec3(0.45f));
        suit_model = glm::rotate(suit_model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(nanosuitShader.modelLoc, 1, GL_FALSE, glm::value_ptr(suit_model));
        glUniformMatrix4fv(nanosuitShader.viewDirLoc, 1, GL_FALSE, glm::value_ptr(cameraFront));
        glUniform1f(nanosuitShader.timeLoc, (float)glfwGetTime());
        nanosuit.Draw(nanosuitShader.id);

        ////////////////////////////////////////////
        glBindVertexArray(0);
        glfwSwapBuffers(window);
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    for (auto shader: color4_shaders) {
        glDeleteProgram(shader.id);
    }
    
    std::cout << "EXERCISE:: exit ok !" << std::endl;
}