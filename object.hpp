#pragma once

#include "camera.hpp"
#include "shader.hpp"
#include "glad/glad.h"
#include "things.hpp"
#include <vector>
#include <glm/ext.hpp>

class Object {
    unsigned vao;
    std::size_t index_count;
    glm::vec3 translation = glm::vec3{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = glm::vec3{0.0f, 0.0f, 0.0f};
    glm::vec3 scaling = glm::vec3{1.0f, 1.0f, 1.0f};
public:
    Object();
    Object(const std::vector<float>& vertices, const std::vector<GLuint>& indices,
        glm::vec3 translation = glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3 rotation = glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3 scale = glm::vec3{0.0f, 0.0f, 0.0f});
    static Object with_texture(const std::vector<float>& vertices, const std::vector<GLuint>& indices, const std::vector<float>& texture_coordinates);
    static Object from_file(const std::string filename);
    void draw(ShaderProgram& program, const std::vector<Light>& lights, const Camera& camera);
    Object& translate(const glm::vec3 distances);
    Object& rotate(const glm::vec3 angles);
    Object& scale(const glm::vec3 ratios);
    
    glm::vec3& get_translation();
    glm::vec3& get_rotation();
    glm::vec3& get_scale();
};
