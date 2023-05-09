#include "shader.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <array>

std::string read_file(std::ifstream& in) {
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}
    
GLuint ShaderProgram::new_location(const std::string& name) {
    GLuint ret;
    if(uniforms.find(name) == uniforms.end()) {
        uniforms[name] = (ret = glGetUniformLocation(program, name.c_str()));
    } else {
        ret = uniforms[name];
    }
    return ret;
}

ShaderProgram ShaderProgram::from_files(const std::string vertex_filename, const std::string fragment_filename) {
    std::ifstream vertex_file(vertex_filename);
    const std::string vertex = read_file(vertex_file);
    std::ifstream frag_file(fragment_filename);
    const std::string frag = read_file(frag_file);
    vertex_file.close();
    frag_file.close();
    ShaderProgram program(vertex, frag);
    return std::move(program);
}

ShaderProgram::ShaderProgram() {
    vertex_shader = 0;
    fragment_shader = 0;
    program = 0;
}

ShaderProgram::ShaderProgram(const std::string vertex_source, const std::string fragment_source) {
    int success;
    std::array<char, BUFSIZ> info_log;
    
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* csource = vertex_source.c_str();
    glShaderSource(vertex_shader, 1, &csource, nullptr);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(not success) {
        glGetShaderInfoLog(vertex_shader, BUFSIZ, nullptr, info_log.data());
        throw std::runtime_error("Vertex Shader Error:\n" + std::string(info_log.data()));
    }
    
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    csource = fragment_source.c_str();
    glShaderSource(fragment_shader, 1, &csource, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(not success) {
        glGetShaderInfoLog(fragment_shader, BUFSIZ, nullptr, info_log.data());
        throw std::runtime_error("Fragment Shader Error:\n" + std::string(info_log.data()));
    }
    
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(not success) {
        glGetProgramInfoLog(program, BUFSIZ, nullptr, info_log.data());
        throw std::runtime_error("Shader Linkage Error:\n" + std::string(info_log.data()));
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void ShaderProgram::use() const {
    glUseProgram(program);
    check_gl("Shader Use");
}

bool ShaderProgram::is_empty() const {
    return program == 0;
}

void ShaderProgram::set(const std::string& name, const float f) {
    glUniform1f(new_location(name), f);
}

void ShaderProgram::set(const std::string& name, const glm::vec4 v) {
    glUniform4f(new_location(name), v.x, v.y, v.z, v.w);
}

void ShaderProgram::set(const std::string& name, const glm::mat4 m) {
    glUniformMatrix4fv(new_location(name), 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::set(const std::string& name, const glm::vec3 v) {
    glUniform3f(new_location(name), v.x, v.y, v.z);
}

void ShaderProgram::set(const std::string& name, const GLuint i) {
    glUniform1i(new_location(name), i);
}
