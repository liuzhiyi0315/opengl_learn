#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // 颜色变量的属性位置值为 1
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec2 aTexCoord2;

out vec3 ourColor; // 向片段着色器输出一个颜色
out vec2 TexCoord;
out vec2 TexCoord2;

uniform vec4 ourDelta; // 在OpenGL程序代码中设定这个变量

void main()
{
    gl_Position = vec4(aPos.x + ourDelta.x, -aPos.y + ourDelta.y, aPos.z, 1.0);
    ourColor = aColor; // 将ourColor设置为我们从顶点数据那里得到的输入颜色
    TexCoord = aTexCoord;
    TexCoord2 = aTexCoord2;
}