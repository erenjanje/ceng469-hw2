#pragma once

#include "glad/glad.h"
#include <glm/ext.hpp>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <array>

#warning "Delete execinfo in production"
#include <execinfo.h>

static std::unordered_map<GLenum, const std::string> error_names = {
    {GL_INVALID_ENUM, "invalid enum"},
    {GL_INVALID_VALUE, "invalid value"},
    {GL_INVALID_OPERATION, "invalid operation"},
    {GL_INVALID_FRAMEBUFFER_OPERATION, "invalid framebuffer operation"},
    {GL_OUT_OF_MEMORY, "out of memory"},
    {GL_STACK_UNDERFLOW, "stack underflow"},
    {GL_STACK_OVERFLOW, "stack overflow"},
};

static void check_gl(const std::string context) {
    int status = glGetError();
    if(status != GL_NO_ERROR) {
        std::array<void*, 10> array;
        std::size_t size = backtrace(array.data(), 10);
        std::cerr << "Backtrace:\n";
        backtrace_symbols_fd(array.data(), size, 2);
        throw std::runtime_error(context + " Error: " + error_names[status]);
    }
}

#define check_gl(ctx) check_gl("(" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + ") " + ctx)

#define LOG(val) std::cout << "(" << __FILE__ << ":" << __LINE__ << ")[" << (#val) << "]" << ": " << (val) << "\n"

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << "[";
    for(const auto& e : v) {
        os << " " << e;
    }
    return os << " ]";
}

static std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    return os << "{" << v.x << ", " << v.y << ", " << v.z << "}";
}

static std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
    return os << "{" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "}";
}
