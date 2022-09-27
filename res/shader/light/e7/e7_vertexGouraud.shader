#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // 颜色变量的属性位置值为 1
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec2 aTexCoord2;
layout (location = 4) in vec3 aNormal;

out vec3 ourColor; // 向片段着色器输出一个颜色
out vec2 TexCoord;
out vec2 TexCoord2;

// out vec3 FragPos;
// out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 VertexLightColor;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    ourColor = aColor; // 将ourColor设置为我们从顶点数据那里得到的输入颜色
    TexCoord = aTexCoord;
    TexCoord2 = aTexCoord2;

    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));

    // -----------------ambient-----------------
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // -----------------diffuse-----------------
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // -----------------specular----------------
    float specularStrength = 1.0;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    VertexLightColor = ambient + diffuse + specular;
}