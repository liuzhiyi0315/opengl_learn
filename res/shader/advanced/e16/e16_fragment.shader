#version 330 core

in vec3 ourColor;
in vec2 TexCoord;
in vec2 TexCoord2;
in vec2 TexCoordEmission;
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform float tex2transparent;
uniform int singleColor;

#define max_tex_cnt 3

struct Material {
    int tex_cnt;
    sampler2D diffuse[max_tex_cnt];
    sampler2D specular[max_tex_cnt];
    float shininess;
}; 

uniform Material material;

uniform sampler2D emission;
uniform sampler2D grass;
uniform sampler2D transparent_window;

uniform vec3 viewPos;

/////////////////////////////////////////////////////////////////////
struct LightParts {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct LightAttenuation {
    float constant;
    float linear;
    float quadratic;
};

struct Light {
    int type;
    vec3 pos;
    vec3  dir;
    float cutOff;
    float outerCutOff;
    LightParts part;
    LightAttenuation attn;
};

uniform Light dirLight;

#define NR_POINT_LIGHTS 4
uniform Light pointLights[NR_POINT_LIGHTS];

uniform Light spotLight;

uniform samplerCube skybox;

vec3 CalcLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir;
    float attenuation = 1.0f;
    float intensity = 1.0f;

    if (light.type == 0) {
        lightDir = normalize(-light.dir);
    }
    else {
        lightDir = normalize(light.pos - fragPos);
        // 衰减
        float distance    = length(lightDir);
        float attenuation = 1.0 / (light.attn.constant + light.attn.linear * distance + 
                    light.attn.quadratic * (distance * distance));
        if (light.type == 2) {
            float theta = dot(lightDir, normalize(-light.dir));
            float epsilon   = light.cutOff - light.outerCutOff;
            intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        }
    }

    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    spec = clamp(spec, 0.0, 1.0);
  
    // 合并结果
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    vec3 diffuse_tex = vec3(mix(texture(material.diffuse[0], TexCoord2), texture(skybox, R), 0.2));
    vec3 specular_tex = vec3(mix(texture(material.specular[0], TexCoord2), texture(skybox, R), 0.5));


    vec3 ambient  = light.part.ambient  * diffuse_tex;
    vec3 diffuse  = light.part.diffuse  * diff * diffuse_tex;
    vec3 specular = light.part.specular * spec * specular_tex;

    ambient  *= attenuation;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}


void main()
{
    // vec4 BaseColor = mix(texture(ourTexture, TexCoord),  vec4(ourColor, 1.0), 0.5);
    // FragColor = mix(BaseColor, texture(ourTexture2, TexCoord2), tex2transparent);
    if (singleColor == 0) {
        // 属性
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);

        // 第一阶段：定向光照
        vec3 result = CalcLight(dirLight, norm, viewDir, FragPos);

        // 第二阶段：点光源
        for(int i = 0; i < NR_POINT_LIGHTS; i++)
            result += CalcLight(pointLights[i], norm, FragPos, viewDir);   

        // 第三阶段：聚光
        result += CalcLight(spotLight, norm, FragPos, viewDir); 

        FragColor = vec4(result, 1.0);
    }
    else if (singleColor == 1){
        FragColor = vec4(0.04, 0.28, 0.26, 1.0);
    }
    else if (singleColor == 3){
        vec4 texColor = texture(grass, TexCoord2);
        if(texColor.a < 0.1)
        {
             discard;
        }
        FragColor = texColor;
        // FragColor = texture(grass, TexCoord2);
    }
    else if (singleColor == 4) {
        FragColor = texture(transparent_window, TexCoord2);
    }
    else if (singleColor == 5) {
        vec3 I = normalize(FragPos - viewPos);
        vec3 R = reflect(I, normalize(Normal));
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
    else if (singleColor == 6) {
        float ratio = 1.00 / 1.52;
        vec3 I = normalize(FragPos - viewPos);
        vec3 R = refract(I, normalize(Normal), ratio);
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
    else {
        FragColor = normalize(vec4(238, 232, 170, 255));
    }

}