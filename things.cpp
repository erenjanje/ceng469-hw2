#include "things.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <array>
#include <glm/geometric.hpp>
#include <stdexcept>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>
#include <unordered_map>
#include "utils.hpp"
#include "shader.hpp"
#include "object.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "cubemap.hpp"

static const auto bottom_floor = std::vector<float>{
    -10.0f, -1.0f,  10.0f,
     10.0f, -1.0f,  10.0f,
     10.0f, -1.0f, -10.0f,
    -10.0f, -1.0f, -10.0f,
};

static const auto bottom_textures = std::vector<float>{
    -10.0f,  10.0f,
     10.0f,  10.0f,
     10.0f, -10.0f,
    -10.0f, -10.0f,
};

static const auto bottom_indices = std::vector<GLuint> {
    0, 1, 3,
    1, 2, 3,
};

static const auto vehicle_vertices = std::vector<float>{
    -1.0f, -1.0f, -1.0f, // 0
     1.0f, -1.0f, -1.0f, // 1
     1.0f,  1.0f, -1.0f, // 2
    -1.0f,  1.0f, -1.0f, // 3
    -1.0f, -1.0f,  1.0f, // 4
     1.0f, -1.0f,  1.0f, // 5
     1.0f,  1.0f,  1.0f, // 6
    -1.0f,  1.0f,  1.0f, // 7
};

static const auto vehicle_indices = std::vector<GLuint>{
    0, 1, 3,
    1, 2, 3,
    1, 5, 2,
    5, 6, 2,
    5, 4, 6,
    4, 7, 6,
    4, 0, 3,
    3, 7, 4,
    3, 2, 7,
    2, 6, 7,
    5, 1, 4,
    1, 0, 4,
};

static struct {
    Object ground;
    Object vehicle;
    Object armadillo;
} objects;

static struct {
    ShaderProgram normal;
    ShaderProgram ground;
    ShaderProgram cubemap;
} programs;

const std::vector<Light> lights = {
    {{2.0f, 3.0f, 5.0f}, {2.0f, 2.0f, 1.0f}}
};

static Camera camera;

static Image ground_image;
static Texture ground_texture;
static Cubemap cubemap;

void init() {
    programs.normal = ShaderProgram::from_files("./normal_vertex.glsl", "./normal_fragment.glsl");
    programs.ground = ShaderProgram::from_files("./ground_vertex.glsl", "./ground_fragment.glsl");
    programs.cubemap = ShaderProgram::from_files("./cubemap_vertex.glsl", "./cubemap_fragment.glsl");
    
    objects.ground = Object::with_texture(bottom_floor, bottom_indices, bottom_textures);
    objects.vehicle = Object(vehicle_vertices, vehicle_indices, glm::vec3{0.0f, 0.0f, -5.0f});
    objects.armadillo = Object::from_file("./armadillo.obj").translate(glm::vec3{0.0f, 0.0f, -10.0f}).rotate(glm::vec3{0.0f, 180.0f, 0.0f});
    
    ground_texture = Texture("./ground.png");
    cubemap = Cubemap(std::vector<std::string>{
        "./cubemap/px.png",
        "./cubemap/nx.png",
        "./cubemap/py.png",
        "./cubemap/ny.png",
        "./cubemap/pz.png",
        "./cubemap/nz.png",
    });
}

void update() {
    programs.cubemap.use();
    objects.vehicle.get_translation() = camera.position;
    glDisable(GL_DEPTH_TEST);
    cubemap.bind();
    // programs.cubemap.set("cube_sampler", cubemap.get_sampler());
    objects.vehicle.draw(programs.cubemap, lights, camera);
    glEnable(GL_DEPTH_TEST);
    cubemap.unbind();
    
    programs.normal.use();
    programs.normal.set("eyepos", camera.position);
    objects.armadillo.draw(programs.normal, lights, camera);

    programs.ground.use();
    ground_texture.bind();
    objects.ground.draw(programs.ground, lights, camera);
}

void key_event(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE and action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    const auto right = glm::normalize(glm::cross(camera.horizontal_direction, camera.up));
    
    switch(key) {
        case GLFW_KEY_A:
            camera.position -= right * 0.1f;
        break;
        case GLFW_KEY_W:
            camera.position += camera.horizontal_direction * 0.1f;
        break;
        case GLFW_KEY_D:
            camera.position += right * 0.1f;
        break;
        case GLFW_KEY_S:
            camera.position -= camera.horizontal_direction * 0.1f;
        break;
        case GLFW_KEY_SPACE:
            camera.position += glm::vec3{0.0f, 0.1f, 0.0f};
        break;
        case GLFW_KEY_LEFT_SHIFT:
            camera.position += glm::vec3{0.0f, -0.1f, 0.0f};
        break;
        
        case GLFW_KEY_LEFT:
            camera.horizontal_direction = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3{0.0f, 1.0f, 0.0f}) * glm::vec4(camera.horizontal_direction, 0.0f);
        break;
        case GLFW_KEY_RIGHT:
            camera.horizontal_direction = glm::rotate(glm::mat4(1.0f), glm::radians(-1.0f), glm::vec3{0.0f, 1.0f, 0.0f}) * glm::vec4(camera.horizontal_direction, 0.0f);
        break;
        case GLFW_KEY_UP:
            camera.t = std::min(std::max(camera.t - 0.01f, 0.0f), 1.0f);
        break;
        case GLFW_KEY_DOWN:
            camera.t = std::max(std::min(camera.t + 0.01f, 1.0f), 0.0f);
        break;
    }
    
    camera.direction = glm::mix(glm::quat{0.0f, glm::normalize(camera.horizontal_direction + glm::vec3{0.0f, 1.0f, 0.0f})}, glm::quat{0.0f, glm::normalize(camera.horizontal_direction + glm::vec3{0.0f, -1.0f, 0.0f})}, camera.t) * camera.horizontal_direction;
}

Image::Image() {}

Image::Image(std::string filename) {
    data = stbi_load(filename.c_str(), &width, &height, &channel_number, 0);
    if(data == nullptr) {
        throw std::runtime_error("Loading Image \"" + filename + "\"");
    }
}

void Image::destroy() {
    stbi_image_free(data);
    width = 0;
    height = 0;
    data = nullptr;
}
