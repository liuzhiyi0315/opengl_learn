#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;

out VS_OUT {
    vec2 texCoords;
    vec3 normal;
    vec3 FragPos;
} vs_out;

uniform bool reverse_normals;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    if(reverse_normals) // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
        vs_out.normal = transpose(inverse(mat3(model))) * (-1.0 * aNormal);
    else
        vs_out.normal = transpose(inverse(mat3(model))) * aNormal;

    vs_out.texCoords = aTexCoords;
    vs_out.FragPos = aPos;
}