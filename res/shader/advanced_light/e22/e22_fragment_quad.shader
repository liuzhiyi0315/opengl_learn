#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube depthMap;

float near = 1.0; 
float far  = 25.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{             
    float depthValue = texture(depthMap, TexCoords).r;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far), 1.0); // perspective
    // FragColor = vec4(vec3(depthValue), 1.0); // orthographic
    // FragColor = texture(depthMap, TexCoords);
    FragColor = vec4(vec3(depthValue), 1.0);
    // FragColor = texture(depthMap, TexCoords);
}