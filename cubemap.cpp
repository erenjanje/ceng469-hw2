#include "cubemap.hpp"

#include "glad/glad.h"
#include "stb_image.h"

Cubemap::Cubemap() {}

Cubemap::Cubemap(const std::vector<std::string>& files) {
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for(std::size_t i = 0; i < 6; i++) {
        int width, height, channel_number;
        unsigned char* data = stbi_load(files[i].c_str(), &width, &height, &channel_number, 0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::draw(ShaderProgram& shader, const glm::vec3 camera_pos) {
    
}

void Cubemap::bind() const {
    glBindSampler(0, sampler);
}

void Cubemap::unbind() const {
    glBindSampler(0, 0);
}

GLuint Cubemap::get_sampler() const {
    return sampler;
}
