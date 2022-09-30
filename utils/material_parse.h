#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
}; 

std::unordered_map<std::string, Material> ParseMaterial(const std::string file_path);