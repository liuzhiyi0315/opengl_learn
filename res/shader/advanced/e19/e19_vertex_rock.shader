#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in vec3 aNormal;
layout (location = 5) in mat4 instanceMatrix;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out VS_OUT {
    vec2 texCoords;
    vec3 normal;
} vs_out;

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * instanceMatrix * vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * instanceMatrix)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
    vs_out.texCoords = aTexCoords;
}