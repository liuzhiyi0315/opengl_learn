#include "shader_parse.h"
#include <GL/glew.h>
#include <iostream>


std::string ParseShader(const std::string file_path) {
    std::fstream stream(file_path);

    std::string line;
    std::stringstream ss;

    while (getline(stream, line))
    {
        /* code */
        ss << line << '\n';
    }
    
    return ss.str();
}

unsigned int createShader(unsigned int shader_type, std::string shader_src) {
    unsigned int shader;
    shader = glCreateShader(shader_type);

    const char* _shader_src = shader_src.c_str();
    glShaderSource(shader, 1, &_shader_src, nullptr);
    glCompileShader(shader);

    int  success;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    
    return shader;
}

unsigned int createShaderProgram(unsigned int shaders[]) {
    auto shader_count = sizeof(shaders) / sizeof(unsigned int);
    auto shaderProgram = glCreateProgram();

    for (auto i = 0; i < shader_count; i ++) {
        glAttachShader(shaderProgram, shaders[i]);
    }
    
    glLinkProgram(shaderProgram);

    int  success;

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    // delete shader after link
    for (auto i = 0; i < shader_count; i ++) {
        glDeleteShader(shaders[i]);
    }

    // glUseProgram(shaderProgram);
    return shaderProgram;
}