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
    glm::vec3* lightPos;
    unsigned int texture;
    unsigned int cube_texture[3];
    Cube* cube;
};

// renders the 3D scene
// --------------------
void _renderScene(const ShaderInfo &shader, RenderInfo& renderInfo) {
    // cube room
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(12.0f));
    glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDisable(GL_CULL_FACE);
    glUniform1i(glGetUniformLocation(shader.id, "reverse_normals"), 1);

    if (UINT_MAX != shader.viewDirLoc) {
        glUniformMatrix4fv(shader.viewDirLoc, 1, GL_FALSE, glm::value_ptr(*(renderInfo.cameraFront)));
        glUniform1i(glGetUniformLocation(shader.id, "material.diffuse"), 0);
        glUniform1i(glGetUniformLocation(shader.id, "material.specular"), 1);
        glUniform1i(glGetUniformLocation(shader.id, "shadowMap"), 2);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderInfo.texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderInfo.texture);
    renderInfo.cube->Render();
    glUniform1i(glGetUniformLocation(shader.id, "reverse_normals"), 0);
    glEnable(GL_CULL_FACE);

    // cubes
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderInfo.cube_texture[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderInfo.cube_texture[1]);
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

    // render light pos
    // if (UINT_MAX != shader.viewDirLoc) {
    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, renderInfo.cube_texture[2]);
    //     model = glm::mat4(1.0f);
    //     model = glm::translate(model, *(renderInfo.lightPos));
    //     model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    //     model = glm::scale(model, glm::vec3(0.1));
    //     glUniformMatrix4fv(shader.modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //     renderInfo.cube->Render();
    // }

}

// void renderScene(glm::mat4& lightSpaceMatrix, const ShaderInfo &shader, RenderInfo& renderInfo)
// {
//     glUseProgram(shader.id);
//     glUniformMatrix4fv(glGetUniformLocation(shader.id, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

//     _renderScene(shader, renderInfo);
// }

void renderOminiScene(const ShaderInfo &shader, RenderInfo& renderInfo)
{
    glUseProgram(shader.id);
    _renderScene(shader, renderInfo);
}

// void renderDepthTexture(glm::mat4& lightSpaceMatrix, ShaderInfo& depthShader, unsigned int depthMapFBO, RenderInfo& renderInfo) {
//     // render scene from light's point of view
//     glUseProgram(depthShader.id);
//     glUniformMatrix4fv(glGetUniformLocation(depthShader.id, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    
//     glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    
//     glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//     glClear(GL_DEPTH_BUFFER_BIT);
//     // glActiveTexture(GL_TEXTURE0);
//     // glBindTexture(GL_TEXTURE_2D, renderInfo.texture);
//     renderScene(lightSpaceMatrix, depthShader, renderInfo);
    
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);

//     // reset viewport
//     glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// }

void renderCubeDepthTexture(std::vector<glm::mat4>& shadowTransforms, ShaderInfo& depthShader, unsigned int depthMapFBO, RenderInfo& renderInfo) {

    // render scene from light's point of view
    glUseProgram(depthShader.id);

    char uniform_name[32]; 
    for (int i = 0; i < 6; i ++) {
        std::sprintf(uniform_name, "shadowMatrices[%d]", i);
        auto loc = glGetUniformLocation(depthShader.id, uniform_name);
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
    }
    
    glUniformMatrix3fv(glGetUniformLocation(depthShader.id, "lightPos"), 1, GL_FALSE, glm::value_ptr(*renderInfo.lightPos));

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, renderInfo.texture);
    renderOminiScene(depthShader, renderInfo);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
} 

// unsigned int createDepthFBO(unsigned int& depthMap, int type=0) {
//     unsigned int depthMapFBO;
//     glGenFramebuffers(1, &depthMapFBO);
//     // create depth texture
//     glGenTextures(1, &depthMap);
//     glBindTexture(GL_TEXTURE_2D, depthMap);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//     GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
//     glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
//     // attach depth texture as FBO's depth buffer
//     glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
//     glDrawBuffer(GL_NONE);
//     glReadBuffer(GL_NONE);
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//     return depthMapFBO;
// }

// void creteLightMatirix(glm::vec3& lightPos, glm::mat4& lightSpaceMatrix) {
//     float near_plane = 1.0f, far_plane = 7.5f;
//     auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//     // auto lightProjection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
//     auto lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
//     lightSpaceMatrix = lightProjection * lightView;
//     return;
// }

// void createCubeMatrices(glm::vec3& lightPos, std::vector<glm::mat4>& shadowTransforms) {
//     // glm::vec3 dir_vec[] = {
//     //     glm::vec3( 1.0,  0.0,  0.0),
//     //     glm::vec3(-1.0,  0.0,  0.0),
//     //     glm::vec3( 0.0,  1.0,  0.0),
//     //     glm::vec3( 0.0, -1.0,  0.0),
//     //     glm::vec3( 0.0,  0.0,  1.0),
//     //     glm::vec3( 0.0,  0.0, -1.0),
//     // };

//     // int up_indices[] = {
//     //     3, 3, 4, 5, 3, 3
//     // };

//     float aspect = (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT;
//     float near = 1.0f;
//     float far = 25.0f;
//     glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far); 

//     // for (int i =0; i < 6; i ++) {
//     //     shadowTransforms.push_back(shadowProj * 
//     //              glm::lookAt(lightPos, lightPos + dir_vec[i], dir_vec[up_indices[i]]));
//     // }
//         shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//         shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//         shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
//         shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
//         shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//         shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
// }

void exercise22(GLFWwindow* window) {
    // object shader program
    ShaderInfo planeShader;

    {
        auto vertexShaderSource = ParseShader("../res/shader/advanced_light/e22/e22_vertex.shader");
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);

        auto fragmentShaderSource = ParseShader("../res/shader/advanced_light/e22/e22_fragment.shader");
        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        
        unsigned int shaders[] = {vertexShader, fragmentShader};
        planeShader.id = createShaderProgram(shaders, 2);
        planeShader.uniformBlockIndex = glGetUniformBlockIndex(planeShader.id, "Matrices");
        glUniformBlockBinding(planeShader.id, planeShader.uniformBlockIndex, 0);
        planeShader.modelLoc = glGetUniformLocation(planeShader.id, "model");
        planeShader.viewDirLoc = glGetUniformLocation(planeShader.id, "viewPos");
    }

    ShaderInfo depthShader;

    {
        auto vertexShaderSource = ParseShader("../res/shader/advanced_light/e22/e22_vertex_depth.shader");
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);

        auto geometryShaderSource = ParseShader("../res/shader/advanced_light/e22/e22_geometry_depth.shader");
        auto geometryShader = createShader(GL_GEOMETRY_SHADER, geometryShaderSource);

        auto fragmentShaderSource = ParseShader("../res/shader/advanced_light/e22/e22_fragment_depth.shader");
        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        
        unsigned int shaders[] = {vertexShader, geometryShader, fragmentShader};
        depthShader.id = createShaderProgram(shaders, 3);
        depthShader.modelLoc = glGetUniformLocation(depthShader.id, "model");
        depthShader.viewDirLoc = UINT_MAX;
    }

    ShaderInfo quadShader;

    {
        auto vertexShaderSource = ParseShader("../res/shader/advanced_light/e22/e22_vertex_quad.shader");
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);

        auto fragmentShaderSource = ParseShader("../res/shader/advanced_light/e22/e22_fragment_quad.shader");
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

    std::string texture_path("../res/texture/wood.png");
    auto wood_texture = createTextureFromFile(texture_path);

    auto cube_texture1 = createTextureFromFile("../res/texture/container2.png");
    auto cube_texture2 = createTextureFromFile("../res/texture/container2_specular.png");

    auto face_texture = createTextureFromFile("../res/texture/emoji-face.png");

     std::vector<std::string> faces
    {
        "../res/texture/skybox/right.jpg",
        "../res/texture/skybox/left.jpg",
        "../res/texture/skybox/top.jpg",
        "../res/texture/skybox/bottom.jpg",
        "../res/texture/skybox/front.jpg",
        "../res/texture/skybox/back.jpg"
    };
    unsigned int skymapTexture = loadCubemap(faces);

    // lighting info
    // -------------
    // glm::vec3 lightPos(-2.0f, 6.0f, -1.0f);
    // glm::vec3 lightPos(-4.0f, 4.0f, -1.0f);
    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

    // glm::mat4 lightSpaceMatrix;
    // creteLightMatirix(lightPos, lightSpaceMatrix);

    // configure depth map FBO
    // -----------------------
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);

    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 
                SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
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
    glUniform1i(glGetUniformLocation(planeShader.id, "material.specular"), 1);
    glUniformMatrix3fv(glGetUniformLocation(planeShader.id, "lightPos"), 1, GL_FALSE, glm::value_ptr(lightPos));

    int blinn = 0;

    Cube cube;
    cube.Bind();

    Quad quad;
    quad.bind();

    RenderInfo renderInfo;
    renderInfo.cameraFront = &cameraPos;
    renderInfo.lightPos = &lightPos;
    renderInfo.texture = wood_texture;
    renderInfo.cube_texture[0] = cube_texture1;
    renderInfo.cube_texture[1] = cube_texture2;
    renderInfo.cube_texture[2] = face_texture;
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

        // 0. create depth cubemap transformation matrices
        // -----------------------------------------------
        float near_plane = 1.0f;
        float far_plane  = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));

        // 1. render scene to depth cubemap
        // --------------------------------
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        glUseProgram(depthShader.id);

        char uniform_name[32]; 
        for (int i = 0; i < 6; i ++) {
            std::sprintf(uniform_name, "shadowMatrices[%d]", i);
            auto loc = glGetUniformLocation(depthShader.id, uniform_name);
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
        }
        
        glUniformMatrix3fv(glGetUniformLocation(depthShader.id, "lightPos"), 1, GL_FALSE, glm::value_ptr(*renderInfo.lightPos));

        renderOminiScene(depthShader, renderInfo);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. render scene as normal 
        // -------------------------
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 视图矩阵
        processCameraInput(window, cameraPos);
        cameraFront = glm::normalize(front);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 

        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // glUseProgram(planeShader.id);

        // render
        // if (blinn) {
        //     glUniform1f(glGetUniformLocation(planeShader.id, "material.shininess"), 32.0);
        // }
        // else {
        //     glUniform1f(glGetUniformLocation(planeShader.id, "material.shininess"), 8.0);
        // }

        // glUniform1i(glGetUniformLocation(planeShader.id, "blinn"), blinn);

        // renderDepthTexture(lightSpaceMatrix, depthShader, depthMapFBO, renderInfo);
        //renderCubeDepthTexture(shadowTransforms, depthShader, depthMapFBO, renderInfo);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        renderOminiScene(planeShader, renderInfo);

        // glUseProgram(quadShader.id);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, depthCubemap);
        // quad.render();

        // glUseProgram(quadShader.id);
        // glActiveTexture(GL_TEXTURE0);
        // glm::mat4 sky_model = glm::mat4(1.0f);
        // // sky_model = glm::translate(sky_model, cameraPos);
        // // sky_model = glm::scale(sky_model, glm::vec3(1.0f));
        // sky_model = glm::rotate(sky_model, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        // glUniformMatrix4fv(glGetUniformLocation(quadShader.id, "model"), 1, GL_FALSE, glm::value_ptr(sky_model));
        // glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        // cube.Render();

        ////////////////////////////////////////////
        glBindVertexArray(0);
        glfwSwapBuffers(window);
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteProgram(planeShader.id);
    glDeleteTextures(1, &wood_texture);

    std::cout << "EXERCISE:: exit ok !" << std::endl;
}