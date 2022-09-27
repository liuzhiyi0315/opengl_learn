#version 330 core

in vec3 ourColor;
in vec2 TexCoord;
in vec2 TexCoord2;
out vec4 FragColor;

// in vec3 Normal;
// in vec3 FragPos;
in vec3 VertexLightColor;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform float tex2transparent;

// uniform vec3 lightColor;
// uniform vec3 lightPos;
// uniform vec3 viewPos;

void main()
{
    // vec4 BaseColor = mix(texture(ourTexture, TexCoord),  vec4(ourColor, 1.0), 0.5);
    // FragColor = mix(BaseColor, texture(ourTexture2, TexCoord2), tex2transparent);

    vec3 result = VertexLightColor * vec3(1.0f, 0.5f, 0.31f);
    FragColor = vec4(result, 1.0);
}