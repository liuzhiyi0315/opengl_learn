#include "../utils/shader_parse.h"
#include "../utils/texture_parse.h"
#include "../utils/material_parse.h"
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

void exercise11(GLFWwindow* window) {

    // auto material_param = ParseMaterial("../res/material/material.csv");

    // object shader program

    // Phong Shading
    auto vertexShaderSource = ParseShader("../res/shader/light/e11/e11_vertex.shader");
    auto fragmentShaderSource = ParseShader("../res/shader/light/e11/e11_fragment.shader");

    // Gouraud Shading
    // auto vertexShaderSource = ParseShader("../res/shader/light/e7/e7_vertexGouraud.shader");
    // auto fragmentShaderSource = ParseShader("../res/shader/light/e7/e7_fragmentGouraud.shader");

    auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    unsigned int shaders[] = {vertexShader, fragmentShader};
    auto shaderProgram = createShaderProgram(shaders);

    // light shader program
    vertexShaderSource = ParseShader("../res/shader/light/e7/e7_vertex_lighting.shader");
    fragmentShaderSource = ParseShader("../res/shader/light/e7/e7_fragment_lighting.shader");

    vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaders[0] = vertexShader;
    shaders[1] = fragmentShader;

    auto lightingShaderProgram = createShaderProgram(shaders);


    float vertices[] = {
    //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 - 纹理坐标2 - 法线
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f , 0.0f,  0.0f, -1.0f, 
     0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.75f,1.0f, 0.0f , 0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.75f, 1.0f, 0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f , 0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f , 0.0f,  0.0f,  1.0f, 
     0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.75f,1.0f, 0.0f , 0.0f,  0.0f,  1.0f, 
     0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f, 0.0f,  0.0f,  1.0f, 
     0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f, 0.0f,  0.0f,  1.0f, 
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.75f, 1.0f, 0.0f, 1.0f, 0.0f,  0.0f,  1.0f, 
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f , 0.0f,  0.0f,  1.0f, 

    -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f ,-1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.75f,1.0f, 0.0f ,-1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f,-1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f,-1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.75f, 1.0f, 0.0f, 1.0f,-1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f ,-1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f , 1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.75f,1.0f, 0.0f , 1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.75f, 1.0f, 0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f , 1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f , 0.0f, -1.0f,  0.0f, 
     0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.75f,1.0f, 0.0f , 0.0f, -1.0f,  0.0f, 
     0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f, 0.0f, -1.0f,  0.0f, 
     0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f, 0.0f, -1.0f,  0.0f, 
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.75f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f, 
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f , 0.0f, -1.0f,  0.0f, 

    -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f , 0.0f,  1.0f,  0.0f, 
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.75f,1.0f, 0.0f , 0.0f,  1.0f,  0.0f, 
     0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f, 0.0f,  1.0f,  0.0f, 
     0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.75f, 0.75f,0.0f, 0.0f, 0.0f,  1.0f,  0.0f, 
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.75f, 1.0f, 0.0f, 1.0f, 0.0f,  1.0f,  0.0f, 
    -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f , 0.0f,  1.0f,  0.0f  
    };

    float lamp_vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    std::string texture_path("../res/texture/container2.png");
    createTextureFromFile(texture_path);

    std::string texture2_path("../res/texture/container2_specular.png");
    createTextureFromFile(texture2_path);

    std::string texture3_path("../res/texture/matrix.jpg");
    createTextureFromFile(texture3_path);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

    /******************************** lighting vao **********************************************************************/
    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lamp_vertices), lamp_vertices, GL_STATIC_DRAW);
    // 设置灯立方体的顶点属性（对我们的灯来说仅仅只有位置数据）
    int light_strider_units = 6;

    // 坐标属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, light_strider_units * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, light_strider_units * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "emission"), 2);


    auto modelLoc = glGetUniformLocation(shaderProgram, "model");
    auto viewLoc = glGetUniformLocation(shaderProgram, "view");
    auto projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    /////////////////////static lights uniform///////////////////////////////////////////////////////
    // 1. Direction Light
    int type = 0;
    glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.type"), type);

    auto lightDir = glm::vec3(2.0f, 2.0f, 2.0f);
    glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.pos"), 1, glm::value_ptr(lightDir));

    auto lightDefAmbient = glm::vec3(0.1f);
    auto lightDefDiffuse =  glm::vec3(0.2f);
    auto lightDefSpecular = glm::vec3(0.5f);

    glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.part.ambient"), 1, glm::value_ptr(lightDefAmbient));
    glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.part.diffuse"), 1, glm::value_ptr(lightDefDiffuse));
    glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.part.specular"), 1, glm::value_ptr(lightDefSpecular));

    // 2. Point Lights
    type = 1;

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    unsigned int pointLightUniformLocs[4][4];

    {
        char uniform_name[32];
        for (int i = 0; i < 4; i ++) {
            std::sprintf(uniform_name, "pointLights[%d].pos", i);
            pointLightUniformLocs[i][0] = glGetUniformLocation(shaderProgram, uniform_name);
            glUniform3fv(pointLightUniformLocs[i][0], 1, glm::value_ptr(pointLightPositions[i]));

            std::sprintf(uniform_name, "pointLights[%d].part.ambient", i);
            pointLightUniformLocs[i][1] = glGetUniformLocation(shaderProgram, uniform_name);
            glUniform3fv(pointLightUniformLocs[i][1], 1, glm::value_ptr(lightDefAmbient));

            std::sprintf(uniform_name, "pointLights[%d].part.diffuse", i);
            pointLightUniformLocs[i][2] = glGetUniformLocation(shaderProgram, uniform_name);
            glUniform3fv(pointLightUniformLocs[i][2], 1, glm::value_ptr(lightDefDiffuse));

            std::sprintf(uniform_name, "pointLights[%d].part.specular", i);
            pointLightUniformLocs[i][3] = glGetUniformLocation(shaderProgram, uniform_name);
            glUniform3fv(pointLightUniformLocs[i][3], 1, glm::value_ptr(lightDefSpecular));

            unsigned int loc;
            float _tmp = 1.0f;
            std::sprintf(uniform_name, "pointLights[%d].attn.constant", i);
            loc = glGetUniformLocation(shaderProgram, uniform_name);
            glUniform1fv(loc, 1, &_tmp);

            _tmp = 0.09f;
            std::sprintf(uniform_name, "pointLights[%d].attn.linear", i);
            loc = glGetUniformLocation(shaderProgram, uniform_name);
            glUniform1fv(loc, 1, &_tmp);

            _tmp = 0.032f;
            std::sprintf(uniform_name, "pointLights[%d].attn.quadratic", i);
            loc = glGetUniformLocation(shaderProgram, uniform_name);
            glUniform1fv(loc, 1, &_tmp);

            std::sprintf(uniform_name, "pointLights[%d].type", i);
            loc = glGetUniformLocation(shaderProgram, uniform_name);
            glUniform1i(loc, type);
        }
    }

    // 2. Spot Light
    type = 2;
    glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.type"), type);

    auto spotColor = glm::vec3(0.0, 0.2, 0.0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.part.ambient"), 1, glm::value_ptr(lightDefAmbient * spotColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.part.diffuse"), 1, glm::value_ptr(lightDefDiffuse * spotColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.part.specular"), 1, glm::value_ptr(lightDefSpecular * spotColor));

    float _tmp = 1.0f;
    glUniform1fv(glGetUniformLocation(shaderProgram, "spotLight.attn.constant"), 1, &_tmp);
    _tmp = 0.09f;
    glUniform1fv(glGetUniformLocation(shaderProgram, "spotLight.attn.linear"), 1, &_tmp);
    _tmp = 0.032f;
    glUniform1fv(glGetUniformLocation(shaderProgram, "spotLight.attn.quadratic"), 1, &_tmp);

    auto cutOff = glm::cos(glm::radians(12.5f));
    glUniform1fv(glGetUniformLocation(shaderProgram, "spotLight.cutOff"), 1, &cutOff);
    auto outerCutOff = glm::cos(glm::radians(17.5f));
    glUniform1fv(glGetUniformLocation(shaderProgram, "spotLight.outerCutOff"), 1, &outerCutOff);

    //////////////////////////////////////////////////////////////////////////////////////////
    glUseProgram(lightingShaderProgram);
    auto lightModelLoc = glGetUniformLocation(lightingShaderProgram, "model");
    auto lightViewLoc = glGetUniformLocation(lightingShaderProgram, "view");
    auto lightProjectionLoc = glGetUniformLocation(lightingShaderProgram, "projection");

    int tex2transparent = 3;
    int rotate = 0;

    glEnable(GL_DEPTH_TEST);

    glm::vec3 cubePositions[] = {
        // glm::vec3( -4.0f,  0.0f, 0.0f),
        // glm::vec3( -2.0f,  0.0f, 0.0f),
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 0.0f,  -2.0f,  0.0f),
        glm::vec3( 4.0f,  -4.0f,  0.0f),
        glm::vec3( 1.0f,  2.0f,  0.0f)
        // glm::vec3( 2.0f,  0.0f, 0.0f),
        // glm::vec3( 4.0f,  0.0f, 0.0f),
        // glm::vec3( -4.0f,  -2.0f, 0.0f),
        // glm::vec3( -2.0f,  -2.0f, 0.0f),
        // glm::vec3( 0.0f,  -2.0f,  0.0f),
        // glm::vec3( 2.0f,  -2.0f, 0.0f),
        // glm::vec3( 4.0f,  -2.0f, 0.0f),
        // glm::vec3( -4.0f,  2.0f, 0.0f),
        // glm::vec3( -2.0f,  2.0f, 0.0f),
        // glm::vec3( 0.0f,  2.0f,  0.0f),
        // glm::vec3( 2.0f,  2.0f, 0.0f),
        // glm::vec3( 4.0f,  2.0f, 0.0f)
        // glm::vec3(-1.5f, -2.2f, -2.5f),  
        // glm::vec3(-3.8f, -2.0f, -12.3f),  
        // glm::vec3( 2.4f, -0.4f, -3.5f),  
        // glm::vec3(-1.7f,  3.0f, -7.5f),  
        // glm::vec3( 1.3f, -2.0f, -2.5f),  
        // glm::vec3( 1.5f,  2.0f, -2.5f), 
        // glm::vec3( 1.5f,  0.2f, -1.5f), 
        // glm::vec3(-1.3f,  1.0f, -1.5f)  
    };


    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

    glfwSetCursorPosCallback(window, mouse_callback);

    while (! glfwWindowShouldClose(window))
	{       
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_END) == GLFW_PRESS) {
             glfwSetWindowShouldClose(window, true);
        }

        if (is_glfw_key_pressed(window, GLFW_KEY_UP)) {
            if (tex2transparent < 10) {
                tex2transparent += 1;
            }
        }    
        
        if (is_glfw_key_pressed(window, GLFW_KEY_DOWN)) {
            if (tex2transparent > 0) {
                tex2transparent -= 1;
            }
        }
    
        // render
        // ------
        glClearColor(0.25f, 0.35f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 投影矩阵
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);

        // 视图矩阵
        processCameraInput(window, cameraPos);
        cameraFront = glm::normalize(front);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 

        //////////////////////////////////////////////////////////////
        glUseProgram(lightingShaderProgram);
        glBindVertexArray(lightVAO);

        // float scaleAmount = static_cast<float>(sin(glfwGetTime()));

        auto light_model = glm::mat4(1.0f);

        glUniformMatrix4fv(lightProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, glm::value_ptr(view));

        for (auto pos: pointLightPositions) {
            light_model = glm::translate(light_model, pos);
            light_model = glm::scale(light_model, glm::vec3(0.2f));
            glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, glm::value_ptr(light_model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        /////////////////////////////////////////////////////////////
        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "tex2transparent"), tex2transparent * 0.1);

        glBindVertexArray(VAO); 

        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));


        /////////////////////dynamic lights uniform///////////////////////////////////////////////////////
        // auto light_x = 1.0f;
        // auto light_y = 1.0f;
        // auto light_z = 1.0f;
        auto light_x = sin(glfwGetTime() * 2.0f) * 0.15;
        auto light_y = sin(glfwGetTime() * 0.7f) * 0.15;
        auto light_z = sin(glfwGetTime() * 1.3f) * 0.15;
        
        auto lightColor = glm::vec3(light_x, light_y, light_z);

        for (auto l=0; l < 4; l ++) {


            glUniform3fv(pointLightUniformLocs[l][1], 1, glm::value_ptr(lightDefAmbient * lightColor));
            glUniform3fv(pointLightUniformLocs[l][2], 1, glm::value_ptr(lightDefDiffuse * lightColor));
            glUniform3fv(pointLightUniformLocs[l][3], 1, glm::value_ptr(lightDefSpecular * lightColor));
        }

        glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.pos"), 1, glm::value_ptr(cameraPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.dir"), 1, glm::value_ptr(cameraFront));

        float _tex_change[2] = {0.0f, static_cast<float>(glfwGetTime())};
        glUniform2fv(glGetUniformLocation(shaderProgram, "tex_change"), 1, _tex_change);


        for (glm::vec3& cube_pos: cubePositions) {
            glm::mat4 model = glm::mat4(1.0f);
            // float scaleAmount = static_cast<float>(sin(glfwGetTime()));
            // if (cube_pos[0] > 0) {
            //     model = glm::translate(model, cube_pos + scaleAmount);
            // }
            // else {
            //     model = glm::translate(model, cube_pos - scaleAmount);
            // }
            model = glm::translate(model, cube_pos);
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // auto material = iter->second;
            // glUniform3fv(glGetUniformLocation(shaderProgram, "material.ambient"), 1, glm::value_ptr(material.ambient));
            // glUniform3fv(glGetUniformLocation(shaderProgram, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
            // glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(material.specular));
            // glUniform1fv(glGetUniformLocation(shaderProgram, "material.shininess"), 1, &material.shininess);
            // iter ++;

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // second box
        // float scaleAmount = static_cast<float>(sin(glfwGetTime()));
        // trans = glm::scale(trans, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
        // trans = glm::translate(trans, glm::vec3(1.0f, -1.5f, 0.0f));
        // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);
    glDeleteProgram(lightingShaderProgram);
}