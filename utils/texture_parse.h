#pragma once
#include <string>
#include <vector>

unsigned int createTextureFromFile(std::string texture_path, bool flip=false);
unsigned int TextureFromFile(const char *path, const std::string &directory);
unsigned int loadCubemap(std::vector<std::string> faces);