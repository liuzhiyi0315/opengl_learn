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

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

uniform Material material;

struct Light {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

uniform Light light;

uniform vec3 viewPos;

void main()
{
    // vec4 BaseColor = mix(texture(ourTexture, TexCoord),  vec4(ourColor, 1.0), 0.5);
    // FragColor = mix(BaseColor, texture(ourTexture2, TexCoord2), tex2transparent);
    
    // -----------------ambient-----------------
    vec3 ambient = light.ambient * material.ambient;

    // -----------------diffuse-----------------
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.pos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.diffuse;

    // -----------------specular----------------
    float specularStrength = 0.5;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = material.specular * spec * light.specular;

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}