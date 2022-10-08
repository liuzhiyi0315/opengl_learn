#version 330 core

in VS_OUT {
    vec2 texCoords;
    vec3 normal;
    vec3 FragPos;
} fs_in;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    float shininess;
}; 

uniform Material material;

uniform vec3 viewDir;
uniform int blinn;

void main()
{
    vec3 Normal = fs_in.normal;
    vec2 TexCoords = fs_in.texCoords;

    // vec3 lightDir = vec3(0.5, 0.5, 0.0);
    vec3 lightPos =  vec3(0.0, 0.0, 0.0);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);

    // 漫反射着色
    float diff = max(dot(Normal, lightDir), 0.0);
    // 镜面光着色
    float spec = 0.0;

    if (blinn == 1) {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(Normal, halfwayDir), 0.0), material.shininess);
    }
    else {
        vec3 reflectDir = reflect(-lightDir, Normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    vec3 diffuse_tex = texture(material.diffuse, TexCoords).rgb;

    vec3 ambient  = vec3(0.05) * diffuse_tex;
    vec3 diffuse  = vec3(1.0) * diff * diffuse_tex;
    vec3 specular = vec3(0.3) * spec;


    FragColor = vec4(ambient + diffuse + specular, 1.0);
}