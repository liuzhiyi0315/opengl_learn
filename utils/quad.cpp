#include "quad.h"
#include <GL/glew.h>

void Quad::bind()
{
    // initialize (if necessary)
    if (quadVAO == 0)
    {
        float quad_vertices[] = {
             1.0f,  1.0f, 0.0f,  1.0, 1.0,// 右上角
             1.0f, -1.0f, 0.0f,  1.0, 0.0,// 右下角
            -1.0f, -1.0f, 0.0f,  0.0, 0.0,// 左下角
            -1.0f,  1.0f, 0.0f,  0.0, 1.0 // 左上角
        };

        unsigned int quad_indices[] = {
            // 注意索引从0开始! 
            // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
            // 这样可以由下标代表顶点组合成矩形

            0, 1, 3, // 第一个三角形
            1, 2, 3  // 第二个三角形
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(2, quadVBO);
        
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadVBO[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
        // 设置植被的顶点属性
        int quad_strider_units = 5;

        // 坐标属性
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, quad_strider_units * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // 纹理属性
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, quad_strider_units * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
}

void Quad::render()
{
    // render Quad
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Quad::Quad()
{
    quadVAO = 0;
}

Quad::~Quad()
{
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(2, quadVBO);
}
