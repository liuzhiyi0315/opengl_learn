#include "texture_parse.h"
#include "stb_image.h"
#include <GL/glew.h>
#include <string>
#include <iostream>


unsigned char* ParseTextureImage(std::string texture_path, int* width, int* height, int* nrChannels) {
    // stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(texture_path.c_str(), width, height, nrChannels, 0);
    return data;
}

void createTexture(int width, int height, unsigned char* data, unsigned int format) {
    static int texture_used = 0;
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + texture_used);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);


    stbi_image_free(data);
    texture_used +=1;

}

void createTextureFromFile(std::string texture_path) {
    int width, height, nrChannels;
    auto data = ParseTextureImage(texture_path, &width, &height, &nrChannels);
    if (data) {
        unsigned int _format;
        if (nrChannels == 4) {
            _format = GL_RGBA;
        }
        else {
            _format = GL_RGB;
        }

        createTexture(width, height, data, _format);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
}