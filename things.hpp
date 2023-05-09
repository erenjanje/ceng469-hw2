#pragma once

#include <glm/ext.hpp>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <string>
#include "stb_image.h"

struct Light {
    glm::vec3 position;
    glm::vec3 intensity;
};

struct Image {
    int width = 0;
    int height = 0;
    int channel_number = 0;
    unsigned char* data = nullptr;
    
    Image();
    Image(std::string filename);
    void destroy();
};

void update();
void init();
void key_event(GLFWwindow* window, int key, int scancode, int action, int mods);
