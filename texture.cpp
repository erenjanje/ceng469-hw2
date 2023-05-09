#include "texture.hpp"

#include "glad/glad.h"
#include <stdexcept>
#include "stb_image.h"
#include "utils.hpp"

Texture::Texture() {
    
}

Texture::Texture(const std::string filename) {
    int width, height, channel_number;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channel_number, 0);
    if(data == nullptr) {
        throw std::runtime_error("Loading Image \"" + filename + "\"");
    }
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    check_gl("texture creation");
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    check_gl("texture write");
    glGenerateMipmap(GL_TEXTURE_2D);
    check_gl("mipmap");
    
    stbi_image_free(data);
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, texture);
    check_gl("texture bind");
}
