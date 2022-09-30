#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // 颜色变量的属性位置值为 1
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec2 aTexCoord2;
layout (location = 4) in vec3 aNormal;

out vec3 ourColor; // 向片段着色器输出一个颜色
out vec2 TexCoord;
out vec2 TexCoord2;
out vec2 TexCoordEmission;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 tex_change;

vec2 rotateUV(vec2 uv, vec2 pivot, float rotation) {
    float cosa = cos(rotation);
    float sina = sin(rotation);
    uv -= pivot;
    return vec2(
        cosa * uv.x - sina * uv.y,
        cosa * uv.y + sina * uv.x 
    ) + pivot;
}

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    ourColor = aColor; // 将ourColor设置为我们从顶点数据那里得到的输入颜色
    TexCoord = aTexCoord;
    TexCoord2 = aTexCoord2;
    TexCoordEmission = rotateUV(aTexCoord2, vec2(0.5), radians(-90.f)) + tex_change;

    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    // FragPos = vec3(view * model * vec4(aPos, 1.0));
    // Normal = mat3(transpose(inverse(view * model))) * aNormal;
}