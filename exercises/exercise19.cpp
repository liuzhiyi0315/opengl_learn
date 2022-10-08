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

glm::mat4* create_rock_matrices(int amount) {
    glm::mat4 *modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // 初始化随机种子    
    float radius = 8.0;
    float offset = 1.5f;
    for(unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(2, -1.0, 0.0));
        // model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.5f, 0.5f, 0.5f));
        // 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.1f; // 让行星带的高度比x和z的宽度要小
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. 缩放：在 0.05 和 0.25f 之间缩放
        float scale = (rand() % 20) / 500.0f + 0.005;
        model = glm::scale(model, glm::vec3(scale));

        // 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. 添加到矩阵的数组中
        modelMatrices[i] = model;
    }
    return modelMatrices;  
}

void exercise19(GLFWwindow* window) {
    struct ShaderInfo
    {
        unsigned int id;
        unsigned int uniformBlockIndex;
        unsigned int modelLoc;
        unsigned int viewDirLoc;
    };

    // object shader program
    ShaderInfo planetShader;

    {
        auto vertexShaderSource = ParseShader("../res/shader/advanced/e19/e19_vertex_planet.shader");
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);

        auto fragmentShaderSource = ParseShader("../res/shader/advanced/e19/e19_fragment_planet.shader");
        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        
        unsigned int shaders[] = {vertexShader, fragmentShader};
        planetShader.id = createShaderProgram(shaders, 2);
        planetShader.uniformBlockIndex = glGetUniformBlockIndex(planetShader.id, "Matrices");
        glUniformBlockBinding(planetShader.id, planetShader.uniformBlockIndex, 0);
        planetShader.modelLoc = glGetUniformLocation(planetShader.id, "model");
        planetShader.viewDirLoc = glGetUniformLocation(planetShader.id, "viewDir");
    }

    ShaderInfo rockShader;

    {
        auto vertexShaderSource = ParseShader("../res/shader/advanced/e19/e19_vertex_rock.shader");
        auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);

        auto fragmentShaderSource = ParseShader("../res/shader/advanced/e19/e19_fragment_planet.shader");
        auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        
        unsigned int shaders[] = {vertexShader, fragmentShader};
        rockShader.id = createShaderProgram(shaders, 2);
        rockShader.uniformBlockIndex = glGetUniformBlockIndex(rockShader.id, "Matrices");
        glUniformBlockBinding(rockShader.id, rockShader.uniformBlockIndex, 0);
        rockShader.modelLoc = glGetUniformLocation(rockShader.id, "model");
        rockShader.viewDirLoc = glGetUniformLocation(rockShader.id, "viewDir");
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

    Model planet;
    planet.loadModel("../res/models/planet/planet.obj");

    const int rock_amount = 2000;
    auto modelMatrices = create_rock_matrices(rock_amount);

    Model rock;
    rock.loadModel("../res/models/rock/rock.obj");
    rock.setupInstanceMatrices(rock_amount, modelMatrices);

    // auto rock_model = glm::mat4(1.0f);
    // auto rock_trans = glm::mat4(1.0f);
    auto planet_pos = glm::vec3(2, -1.0, 0.0);
    // auto planet_pos = glm::vec3(0.0, 0.0, 0.0);
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

        glUseProgram(planetShader.id);
        
        auto glfw_time = (float)glfwGetTime();

        auto _planet_pos = glm::vec3(projection * view * glm::vec4(planet_pos, 1.0));

        glm::mat4 suit_model = glm::mat4(1.0f);
        suit_model = glm::scale(suit_model, glm::vec3(0.45f));
        suit_model = glm::translate(suit_model, planet_pos);
        suit_model = glm::rotate(suit_model, glfw_time * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(planetShader.modelLoc, 1, GL_FALSE, glm::value_ptr(suit_model));
        glUniformMatrix4fv(planetShader.viewDirLoc, 1, GL_FALSE, glm::value_ptr(cameraFront));
        planet.Draw(planetShader.id);

        // for (auto i = 0; i < rock_amount; i ++) {
        //     auto rock_model = modelMatrices[i];
        //     rock_model = glm::rotate(rock_model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.2f, 1.0f, 0.0f));
        //     glUniformMatrix4fv(planetShader.modelLoc, 1, GL_FALSE, glm::value_ptr(rock_model));
        //     rock.Draw(planetShader.id);
        // }
        glUseProgram(rockShader.id);
        glUniformMatrix4fv(rockShader.viewDirLoc, 1, GL_FALSE, glm::value_ptr(cameraFront));

        auto rock_model = glm::mat4(1.0f);
        rock_model = glm::translate(rock_model, planet_pos);

        auto angle = glfw_time * 30.0f;
        auto rotate_x = 0.2;
        rock_model = glm::rotate(rock_model, glm::radians(angle), glm::vec3(rotate_x, 1.0f, 0.0f));
        // rock_model = glm::rotate(rock_model, glm::radians(angle), glm::vec3(-2.0f, 0.0f, 0.0f));
        // rock_model = glm::translate(rock_model, glm::vec3(2, -1.0, 0.0));
        // RotateArbitraryLine(rock_model, glm::vec3(-2.0, 1.0, 0.0), glm::vec3(2.0, -1.0, 0.0), (float)glfwGetTime() * glm::radians(50.0f));
        glUniformMatrix4fv(rockShader.modelLoc, 1, GL_FALSE, glm::value_ptr(rock_model));
        rock.Draw(rockShader.id, rock_amount);

        ////////////////////////////////////////////
        glBindVertexArray(0);
        glfwSwapBuffers(window);
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteProgram(planetShader.id);
    
    std::cout << "EXERCISE:: exit ok !" << std::endl;
}