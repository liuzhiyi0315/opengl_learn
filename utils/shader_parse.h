#pragma once

#include <string>
#include <fstream>
#include <sstream>

std::string ParseShader(const std::string file_path);

unsigned int createShader(unsigned int shader_type, std::string shader_src);

unsigned int createShaderProgram(unsigned int shaders[]);