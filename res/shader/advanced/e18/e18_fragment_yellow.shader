#version 330 core
in vec3 FaceColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(FaceColor.r, FaceColor.g, FaceColor.b, 0.5);
}