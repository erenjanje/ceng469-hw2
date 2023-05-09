#include <memory>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "drawable.hpp"
#include "things.hpp"

#define CHECK(expr, error_val) check_error<typeof(expr)>(expr, error_val, __FILE__, __LINE__, __FUNCTION__)

template<typename T>
T check_error(T value, const T& error_val, const char* filename, int line, const char* funcname) {
    if(value == error_val) {
        std::fprintf(stderr, "Error in\n   File: \"%s\"\n   Line: %d\n   Function: \"%s\"\n", filename, line, funcname);
        std::exit(EXIT_FAILURE);
    }
    return value;
}

static void glfw_error_cb(int error, const char* description) {
    std::fprintf(stderr, "GLFW Error %d:\n   %s\n", error, description);
}

static void glfw_key_cb(GLFWwindow* window, int key, int scancode, int action, int index) {
    if(key == GLFW_KEY_ESCAPE and action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

const int width = 800;
const int height = 800;
#include <algorithm>

int main() {
    glfwSetErrorCallback(glfw_error_cb);
    CHECK(glfwInit(), GLFW_FALSE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    auto* window = CHECK(glfwCreateWindow(width, height, "Window", nullptr, nullptr), nullptr);
    glfwSetKeyCallback(window, key_event);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    int w, h;
    
    init();
    while(not glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        update();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
