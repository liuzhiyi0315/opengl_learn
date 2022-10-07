#version 330 core

in VS_OUT {
    vec2 texCoords;
    vec3 normal;
} fs_in;

out vec4 FragColor;

#define max_tex_cnt 3

struct Material {
    int tex_cnt;
    sampler2D diffuse[max_tex_cnt];
    sampler2D specular[max_tex_cnt];
    float shininess;
}; 

uniform Material material;

uniform vec3 viewDir;

void main()
{
    vec3 Normal = fs_in.normal;
    vec2 TexCoords = fs_in.texCoords;

    vec3 lightDir = vec3(0.5, 0.5, 0.0);
    // 漫反射着色
    float diff = max(dot(Normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    spec = clamp(spec, 0.0, 1.0);

    vec3 diffuse_tex = vec3(texture(material.diffuse[0], TexCoords)) + vec3(texture(material.diffuse[1], TexCoords)) + vec3(texture(material.diffuse[2], TexCoords));
    vec3 specular_tex = vec3(texture(material.specular[0], TexCoords)) + vec3(texture(material.specular[1], TexCoords)) + vec3(texture(material.specular[2], TexCoords));

    vec3 ambient  = vec3(0.1) * diffuse_tex;
    vec3 diffuse  = vec3(0.3) * diff * diffuse_tex;
    vec3 specular = vec3(0.6) * spec * specular_tex;


    FragColor = vec4(ambient + diffuse + specular, 1.0);
}