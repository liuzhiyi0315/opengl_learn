#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in vec3 aNormal;

uniform mat4 model;
out vec2 TexCoords;

void main()
{
    gl_Position = model * vec4(aPos, 1.0f); 
    TexCoords = aTexCoords;
}