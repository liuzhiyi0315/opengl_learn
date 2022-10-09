#include "../utils/shader_parse.h"
#include "../utils/texture_parse.h"
#include "../utils/material_parse.h"
#include "../utils/model.h"
#include "../utils/cube.h"
#include "../utils/quad.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SHADOW_WIDTH 1600
#define SHADOW_HEIGHT 1600
#define SCR_WIDTH 1024
#define SCR_HEIGHT 768

bool is_glfw_key_pressed(GLFWwindow* window, int key);

double get_frame_delta();

void processCameraInput(GLFWwindow *window, glm::vec3& cameraPos);

extern glm::vec3 front;

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

struct ShaderInfo
{
    unsigned int id;
    unsigned int uniformBlockIndex;
    unsigned int modelLoc;
    unsigned int viewDirLoc;
};

struct RenderInfo {
    glm::vec3* cameraFront;
    unsigned int texture;
    unsigned int vao;
    Cube* cube;
};

// renders the 3D scene
// --------------------
void renderScene(glm::mat4& lightSpaceMatrix, const ShaderInfo &shader, RenderInfo& renderInfo)
{
    glUseProgram(shader.id);
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    // floor
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    if (UINT_MAX != shader.viewDirLoc) {
        glUniformMatrix4fv(shader.viewDirLoc, 1, GL_FALSE, glm::value_ptr(*(renderInfo.cameraFront)));
        glUniform1i(glGetUniformLocation(shader.id, "material.diffuse"), 0);
        glUniform1i(glGetUniformLocation(shader.id, "shadowMap"), 1);
    }
    glBindVertexArray(renderInfo.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderInfo.texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    renderInfo.cube->Render();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    renderInfo.cube->Render();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    renderInfo.cube->Render();
}

void renderDepthTexture(glm::mat4& lightSpaceMatrix, ShaderInfo& depthShader, unsigned int depthMapFBO, RenderInfo& renderInfo) {
    // render scene from light's point of view
    glUseProgram(depthShader.id);
    glUniformMatrix4fv(glGetUniformLocation(depthShader.id, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, renderInfo.texture);
    renderScene(lightSpaceMatrix, depthShader, renderInfo);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void exercise21(GLFWwindow* window) {
    // object shader program
    ShaderInfo planeShader;

    {
        auto vertexShaderSource = ParseShader("../res/shader/advanced_light/e21/e21_vertex.shader");
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);

        auto fragmentShaderSource = ParseShader("../res/shader/advanced_light/e21/e21_fragment.shader");
        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        
        unsigned int shaders[] = {vertexShader, fragmentShader};
        planeShader.id = createShaderProgram(shaders, 2);
        planeShader.uniformBlockIndex = glGetUniformBlockIndex(planeShader.id, "Matrices");
        glUniformBlockBinding(planeShader.id, planeShader.uniformBlockIndex, 0);
        planeShader.modelLoc = glGetUniformLocation(planeShader.id, "model");
        planeShader.viewDirLoc = glGetUniformLocation(planeShader.id, "viewDir");
    }

    ShaderInfo depthShader;

    {
        auto vertexShaderSource = ParseShader("../res/shader/advanced_light/e21/e21_vertex_depth.shader");
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);

        auto fragmentShaderSource = ParseShader("../res/shader/advanced_light/e21/e21_fragment_depth.shader");
        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        
        unsigned int shaders[] = {vertexShader, fragmentShader};
        depthShader.id = createShaderProgram(shaders, 2);
        depthShader.modelLoc = glGetUniformLocation(depthShader.id, "model");
        depthShader.viewDirLoc = UINT_MAX;
    }

    ShaderInfo quadShader;

    {
        auto vertexShaderSource = ParseShader("../res/shader/advanced_light/e21/e21_vertex_quad.shader");
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);

        auto fragmentShaderSource = ParseShader("../res/shader/advanced_light/e21/e21_fragment_quad.shader");
        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        
        unsigned int shaders[] = {vertexShader, fragmentShader};
        quadShader.id = createShaderProgram(shaders, 2);
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

    // lighting info
    // -------------
    // glm::vec3 lightPos(-2.0f, 6.0f, -1.0f);
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

    float near_plane = 1.0f, far_plane = 7.5f;
    auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    // auto lightProjection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
    auto lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    auto lightSpaceMatrix = lightProjection * lightView;
    
    // configure depth map FBO
    // -----------------------
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    // glEnable(GL_MULTISAMPLE); // Enabled by default on some drivers, but not all so always enable to make sure
    glEnable(GL_DEPTH_TEST);

   
    glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 10.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);


    glUseProgram(planeShader.id);
    glUniform1i(glGetUniformLocation(planeShader.id, "material.diffuse"), 0);
    glUniformMatrix3fv(glGetUniformLocation(planeShader.id, "lightPos"), 1, GL_FALSE, glm::value_ptr(lightPos));

    int blinn = 0;

    Cube cube;
    cube.Bind();

    Quad quad;
    quad.bind();

    RenderInfo renderInfo;
    renderInfo.cameraFront = &cameraFront;
    renderInfo.texture = wood_texture;
    renderInfo.vao = planeVAO;
    renderInfo.cube = &cube;

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
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 视图矩阵
        processCameraInput(window, cameraPos);
        cameraFront = glm::normalize(front);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 

        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glUseProgram(planeShader.id);

        // render
        if (blinn) {
            glUniform1f(glGetUniformLocation(planeShader.id, "material.shininess"), 32.0);
        }
        else {
            glUniform1f(glGetUniformLocation(planeShader.id, "material.shininess"), 8.0);
        }

        glUniform1i(glGetUniformLocation(planeShader.id, "blinn"), blinn);

        renderDepthTexture(lightSpaceMatrix, depthShader, depthMapFBO, renderInfo);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        renderScene(lightSpaceMatrix, planeShader, renderInfo);

        // glUseProgram(depthShader.id);
        // glUniformMatrix4fv(glGetUniformLocation(depthShader.id, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        // renderScene(depthShader, renderInfo);

        // cubes
        // auto model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        // model = glm::scale(model, glm::vec3(0.5f));
        // glUniformMatrix4fv(planeShader.modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // glUseProgram(quadShader.id);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, depthMap);
        // quad.render();

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