#pragma once

#include <unordered_map>
#include <string>
#include "glad/glad.h"
#include <glm/ext.hpp>
#include <exception>
#include <vector>

class ShaderProgram {
    unsigned vertex_shader;
    unsigned fragment_shader;
    unsigned program;
    std::unordered_map<std::string, GLuint> uniforms = std::unordered_map<std::string, GLuint>();
    GLuint new_location(const std::string& name);
public:
    static ShaderProgram from_files(const std::string vertex_filename, const std::string fragment_filename);
    ShaderProgram();
    ShaderProgram(const std::string vertex_source, const std::string fragment_source);
    void use() const;
    bool is_empty() const;
    void set(const std::string& name, const float f);
    void set(const std::string& name, const glm::vec4 v);
    void set(const std::string& name, const glm::mat4 m);
    void set(const std::string& name, const glm::vec3 v);
    void set(const std::string& name, const GLuint i);
    void set(const std::string& name, const std::vector<GLuint>& v);
};


