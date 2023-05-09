#pragma once

#include "shader.hpp"
#include <vector>
#include <string>
#include <glm/ext.hpp>

class Cubemap {
    unsigned texture = 0;
    unsigned sampler = 0;
public:
    Cubemap();
    Cubemap(const std::vector<std::string>& files);
    
    void draw(ShaderProgram& shader, const glm::vec3 camera_pos);
    void bind() const;
    void unbind() const;
    GLuint get_sampler() const;
};
