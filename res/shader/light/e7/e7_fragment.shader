#version 330 core

in vec3 ourColor;
in vec2 TexCoord;
in vec2 TexCoord2;
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform float tex2transparent;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // vec4 BaseColor = mix(texture(ourTexture, TexCoord),  vec4(ourColor, 1.0), 0.5);
    // FragColor = mix(BaseColor, texture(ourTexture2, TexCoord2), tex2transparent);
    
    // -----------------ambient-----------------
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // -----------------diffuse-----------------
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // -----------------specular----------------
    float specularStrength = 0.5;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * vec3(1.0f, 0.5f, 0.31f);
    FragColor = vec4(result, 1.0);
}