#version 330 core

out vec4 FragColor;

in vec3 fColor;

void main()
{
    FragColor = vec4(fColor.r, fColor.g, fColor.b, 1.0);
}