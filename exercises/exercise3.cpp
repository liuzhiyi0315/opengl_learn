#include "../utils/shader_parse.h"
#include "../utils/texture_parse.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void exercise3(GLFWwindow* window) {
    auto vertexShaderSource = ParseShader("../res/shader/e3_vertex.shader");
    auto fragmentShaderSource = ParseShader("../res/shader/e3_fragment.shader");

    auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    unsigned int shaders[] = {vertexShader, fragmentShader};
    auto shaderProgram = createShaderProgram(shaders);

    int vertexDeltaLocation = glGetUniformLocation(shaderProgram, "ourDelta");

    float vertices[] = {
    //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 - 纹理坐标2
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   2.0f, 2.0f,// 右上
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.75f,  2.0f, 0.0f, // 右下
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.75f, 0.75f,0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.75f, 1.0f, 0.0f, 2.0f  // 左上
    };

    unsigned int indices[] = {
        // 注意索引从0开始! 
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形

        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    std::string texture_path("../res/texture/textures.jpg");
    createTextureFromFile(texture_path);

    std::string texture2_path("../res/texture/emoji-face.png");
    createTextureFromFile(texture2_path);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    int strider_units = 10;

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

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture2"), 1);

    int vertex_x_update = 0;
    auto lastTime = glfwGetTime();
    auto lastPress = lastTime;
    int tex2transparent = 3;

    while (! glfwWindowShouldClose(window))
	{

        auto now = glfwGetTime();
        auto delta = now - lastTime;

        if (delta > 0.5) {
            lastTime = now;
            if (vertex_x_update > 10) {
                vertex_x_update = 0;
            }
            else {
                vertex_x_update ++;
            }

        }

        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_END) == GLFW_PRESS) {
             glfwSetWindowShouldClose(window, true);
        }
        else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            if ((now - lastPress) > 0.5) {
                lastPress = now;
                if (tex2transparent < 10) {
                    tex2transparent += 1;
                }
            }
        }    
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            if ((now - lastPress) > 0.5) {
                lastPress = now;
                if (tex2transparent > 0) {
                    tex2transparent -= 1;
                }
            }
        }    
    
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "tex2transparent"), tex2transparent * 0.1);

        auto vertex_x_delta = -0.5f + 0.1f * vertex_x_update;
        glUniform4f(vertexDeltaLocation, vertex_x_delta, vertex_x_delta, 0.0f, 1.0f);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
        glBindVertexArray(0);

        glfwSwapBuffers(window);
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}
