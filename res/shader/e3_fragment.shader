#version 330 core

in vec3 ourColor;
in vec2 TexCoord;
in vec2 TexCoord2;
out vec4 FragColor;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform float tex2transparent;

void main()
{
    vec4 BaseColor = mix(texture(ourTexture, TexCoord),  vec4(ourColor, 1.0), 0.5);
    FragColor = mix(BaseColor, texture(ourTexture2, TexCoord2), tex2transparent);
} 