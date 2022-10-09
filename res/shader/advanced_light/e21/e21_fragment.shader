#version 330 core

in VS_OUT {
    vec2 texCoords;
    vec3 normal;
    vec3 FragPos;
    vec4 FragPosLightSpace;
} fs_in;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    float shininess;
}; 

uniform Material material;
uniform sampler2D shadowMap;

uniform vec3 viewDir;
uniform int blinn;

uniform vec3 lightPos;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}


void main()
{
    vec3 Normal = fs_in.normal;
    vec2 TexCoords = fs_in.texCoords;

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

    vec3 ambient  = vec3(0.3);
    vec3 diffuse  = vec3(1.0) * diff;
    vec3 specular = vec3(0.5) * spec;

     // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, lightDir);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * diffuse_tex;    

    FragColor = vec4(lighting, 1.0);
}