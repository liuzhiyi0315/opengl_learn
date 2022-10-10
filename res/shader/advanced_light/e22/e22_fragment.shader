#version 330 core

in VS_OUT {
    vec2 texCoords;
    vec3 normal;
    vec3 FragPos;
} fs_in;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

uniform Material material;
uniform samplerCube shadowMap;

uniform vec3 viewPos;
uniform int blinn;

uniform vec3 lightPos;

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(shadowMap, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= 25.0;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    // if(fragToLight.z > 1.0)
    //     shadow = 0.0;

    return shadow;
}  

float getClosesDepth(vec3 fragPos) {
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(shadowMap, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= 25.0;
    return closestDepth;
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
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    if (blinn == 1) {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(Normal, halfwayDir), 0.0), material.shininess);
    }
    else {
        vec3 reflectDir = reflect(-lightDir, Normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    vec3 diffuse_tex = texture(material.diffuse, TexCoords).rgb;
    // vec3 specular_tex = texture(material.specular, TexCoords).rgb;

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPos);     

    vec3 ambient  = vec3(0.3) * diffuse_tex;
    vec3 diffuse  = vec3(0.6) * diff * (1.0 - shadow) * diffuse_tex;
    vec3 specular = vec3(0.8) * spec * (1.0 - shadow) * diffuse_tex;

    vec3 lighting = ambient + diffuse;    

    FragColor = vec4(lighting, 1.0);
    // float closestDepth = getClosesDepth(fs_in.FragPos);
    // FragColor = vec4(vec3(closestDepth / 25.0), 1.0);
}