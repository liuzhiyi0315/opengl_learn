#include "../utils/shader_parse.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void exercise1(GLFWwindow* window) {
    auto vertexShaderSource = ParseShader("../res/shader/basic_vertex.shader");
    auto fragmentShaderSource = ParseShader("../res/shader/basic_fragment.shader");
    auto fragmentShaderSource2 = ParseShader("../res/shader/basic_fragment2.shader");

    auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    unsigned int shaders[] = {vertexShader, fragmentShader};
    auto shaderProgram = createShaderProgram(shaders);

    auto vertexShader2 = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    auto fragmentShader2 = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource2);

    unsigned int shaders2[] = {vertexShader, fragmentShader2};
    auto shaderProgram2 = createShaderProgram(shaders2);

    float vertices[] = {
        0.1f + 0.3f, 0.1f, 0.0f,   // 右上角
        0.1f + 0.3f, -0.1f, 0.0f,  // 右下角
        -0.1f + 0.3f, -0.1f, 0.0f, // 左下角
        -0.1f + 0.3f, 0.1f, 0.0f   // 左上角
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


    float vertices2[] = {
        -0.1f -0.3f, -0.1f, 0.0f,
        0.1f - 0.3f, -0.1f, 0.0f,
        0.0f - 0.3f,  0.1f, 0.0f
    };


    unsigned int VBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // 2nd object
    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    while (! glfwWindowShouldClose(window))
	{
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_END) == GLFW_PRESS) {
             glfwSetWindowShouldClose(window, true);
             
        }
           
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
	}

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO2);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(shaderProgram2);
}