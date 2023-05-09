#pragma once 

#include <glm/ext.hpp>

struct Camera {
    float fov = 45.0f;
    float aspect_ratio = 1.0f;
    float near = 0.5f;
    float far = 150.0f;
    glm::vec3 position = glm::vec3{0.0f, 1.0f, 0.0f};
    glm::vec3 direction = glm::vec3{0.0f, 0.0f, -1.0f};
    glm::vec3 horizontal_direction = glm::vec3{0.0f, 0.0f, -1.0f};
    glm::vec3 up = glm::vec3{0.0f, 1.0f, 0.0f};
    float t = 0.5f;
};
