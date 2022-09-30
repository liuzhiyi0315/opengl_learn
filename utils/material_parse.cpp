#include "material_parse.h"

std::unordered_map<std::string, Material> ParseMaterial(const std::string file_path) {
    std::ifstream inFile(file_path, std::ios::in);
    std::string line;

    std::unordered_map<std::string, Material> result;

    float value[10];

    while (getline(inFile, line))
    {
        std::stringstream ss(line);
        std::string str;
        std::string key;

        short i = 0;
        while(getline(ss, str, ',')) {
            if (i ==0) {
                key = str;
            }
            else {
                value[i-1] = std::stof(str);
            }
            i ++;
        }

        Material material;
        material.ambient.x = value[0];
        material.ambient.y = value[1];
        material.ambient.z = value[2];
        material.diffuse.x = value[3];
        material.diffuse.y = value[4];
        material.diffuse.z = value[5];
        material.specular.x = value[6];
        material.specular.y = value[7];
        material.specular.z = value[8];
        material.shininess = value[9];

        result[key] = material;
    }
    return result;
}