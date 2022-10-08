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

extern glm::vec3 front;

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void exercise20(GLFWwindow* window) {
    struct ShaderInfo
    {
        unsigned int id;
        unsigned int uniformBlockIndex;
        unsigned int modelLoc;
        unsigned int viewDirLoc;
    };

    // object shader program
    ShaderInfo planeShader;

    {
        auto vertexShaderSource = ParseShader("../res/shader/advanced/e20/e20_vertex.shader");
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);

        auto fragmentShaderSource = ParseShader("../res/shader/advanced/e20/e20_fragment.shader");
        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        
        unsigned int shaders[] = {vertexShader, fragmentShader};
        planeShader.id = createShaderProgram(shaders, 2);
        planeShader.uniformBlockIndex = glGetUniformBlockIndex(planeShader.id, "Matrices");
        glUniformBlockBinding(planeShader.id, planeShader.uniformBlockIndex, 0);
        planeShader.modelLoc = glGetUniformLocation(planeShader.id, "model");
        planeShader.viewDirLoc = glGetUniformLocation(planeShader.id, "viewDir");
    }

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float planeVertices[] = {
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    std::string texture_path("../res/texture/wood.png");
    auto wood_texture = createTextureFromFile(texture_path);

    // 投影矩阵
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
    // projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE); // Enabled by default on some drivers, but not all so always enable to make sure
    glEnable(GL_DEPTH_TEST);

   
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);


    glUseProgram(planeShader.id);
    glUniform1i(glGetUniformLocation(planeShader.id, "material.diffuse"), 0);

    int blinn = 0;
    ////////////////////////////////////////////////////////////////////

    glfwSetCursorPosCallback(window, mouse_callback);

    while (! glfwWindowShouldClose(window))
	{       
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_END) == GLFW_PRESS) {
             glfwSetWindowShouldClose(window, true);
        }
    
        if (is_glfw_key_pressed(window, GLFW_KEY_B)) {
            blinn = !blinn;
             if (blinn) {
                std::cout << "EXERCISE:: blinn enabled !" << std::endl;
             }
             else {
                std::cout << "EXERCISE:: phong enabled !" << std::endl;
             }
        }

        // render
        // ------
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // 视图矩阵
        processCameraInput(window, cameraPos);
        cameraFront = glm::normalize(front);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 

        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glUseProgram(planeShader.id);
        
        // auto glfw_time = (float)glfwGetTime();

        // auto _planet_pos = glm::vec3(projection * view * glm::vec4(planet_pos, 1.0));

        // glm::mat4 suit_model = glm::mat4(1.0f);
        // suit_model = glm::scale(suit_model, glm::vec3(0.45f));
        // suit_model = glm::translate(suit_model, planet_pos);
        // suit_model = glm::rotate(suit_model, glfw_time * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // glUniformMatrix4fv(planetShader.modelLoc, 1, GL_FALSE, glm::value_ptr(suit_model));
        // glUniformMatrix4fv(planetShader.viewDirLoc, 1, GL_FALSE, glm::value_ptr(cameraFront));
        // planet.Draw(planetShader.id);

        // floor
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(planeShader.modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(planeShader.viewDirLoc, 1, GL_FALSE, glm::value_ptr(cameraFront));

        if (blinn) {
            glUniform1f(glGetUniformLocation(planeShader.id, "material.shininess"), 32.0);
        }
        else {
            glUniform1f(glGetUniformLocation(planeShader.id, "material.shininess"), 8.0);
        }

        glUniform1i(glGetUniformLocation(planeShader.id, "blinn"), blinn);

        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wood_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ////////////////////////////////////////////
        glBindVertexArray(0);
        glfwSwapBuffers(window);
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteProgram(planeShader.id);
    glDeleteTextures(1, &wood_texture);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    
    std::cout << "EXERCISE:: exit ok !" << std::endl;
}