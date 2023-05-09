#include "things.hpp"
#include "framebuffer.hpp"
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
#include <unordered_set>
#include "utils.hpp"
#include "shader.hpp"
#include "object.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "cubemap.hpp"

extern const int width;
extern const int height;

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

static const auto car1_vertices = std::vector<float>{
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -0.5f, -0.5f, 2.0f,
     0.5f, -0.5f, 2.0f,
     0.5f,  0.5f, 2.0f,
    -0.5f,  0.5f, 2.0f,
};

static const auto car1_indices = std::vector<GLuint>{
    3, 1, 0,
    3, 2, 1,
    2, 5, 1,
    2, 6, 5,
    6, 4, 5,
    6, 7, 4,
    3, 0, 4,
    4, 7, 3,
    7, 2, 3,
    7, 6, 2,
    4, 1, 5,
    4, 0, 1,
};

static const auto car2_vertices = std::vector<float>{
    -0.5f, -0.5f, -2.0f,
     0.5f, -0.5f, -2.0f,
     0.5f,  0.5f, -2.0f,
    -0.5f,  0.5f, -2.0f,
    -1.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  0.0f,
     1.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,  0.0f,
};

static const auto car2_indices = std::vector<GLuint>{
    3, 1, 0,
    3, 2, 1,
    2, 5, 1,
    2, 6, 5,
    6, 4, 5,
    6, 7, 4,
    3, 0, 4,
    4, 7, 3,
    7, 2, 3,
    7, 6, 2,
    4, 1, 5,
    4, 0, 1,
};

struct CarObject {
    Object body;
    Object windows;
    
    void draw(ShaderProgram& shader, const std::vector<Light>& lights, const Camera& camera) {
        body.draw(shader, lights, camera);
        windows.draw(shader, lights, camera);
    }
};

static struct {
    Object ground;
    Object vehicle;
    Object armadillo;
    CarObject car;
} objects;

static struct {
    ShaderProgram normal;
    ShaderProgram ground;
    ShaderProgram cubemap;
    ShaderProgram car;
} programs;

const std::vector<Light> lights = {
    {{2.0f, 3.0f, 5.0f}, {2.0f, 2.0f, 1.0f}}
};

static Camera camera;

static Image ground_image;
static Texture ground_texture;
static Cubemap cubemap;
static std::array<Framebuffer, 6> framebuffers;
static std::vector<GLuint> framebuffer_samplers;

void init() {
    programs.normal = ShaderProgram::from_files("./shaders/normal_vertex.glsl", "./shaders/normal_fragment.glsl");
    programs.ground = ShaderProgram::from_files("./shaders/ground_vertex.glsl", "./shaders/ground_fragment.glsl");
    programs.cubemap = ShaderProgram::from_files("./shaders/cubemap_vertex.glsl", "./shaders/cubemap_fragment.glsl");
    programs.car = ShaderProgram::from_files("./shaders/car_vertex.glsl", "./shaders/car_fragment.glsl");
    
    objects.ground = Object::with_texture(bottom_floor, bottom_indices, bottom_textures);
    objects.vehicle = Object(vehicle_vertices, vehicle_indices, glm::vec3{0.0f, 0.0f, -5.0f});
    objects.armadillo = Object::from_file("./assets/armadillo.obj").translate(glm::vec3{0.0f, 0.0f, -10.0f}).rotate(glm::vec3{0.0f, 180.0f, 0.0f});
    objects.car.body = Object(car1_vertices, car1_indices);
    objects.car.windows = Object(car2_vertices, car2_indices);
    
    objects.car.body.get_translation() = glm::vec3{0.0f, 3.0f, -7.0f};
    objects.car.windows.get_translation() = glm::vec3{0.0f, 3.0f, -7.0f};
    objects.car.body.get_scale() = glm::vec3{0.2f, 0.2f, 0.2f};
    objects.car.windows.get_scale() = glm::vec3{0.2f, 0.2f, 0.2f};
    
    ground_texture = Texture("./ground.png");
    cubemap = Cubemap(std::vector<std::string>{
        "./cubemap/px.png",
        "./cubemap/nx.png",
        "./cubemap/py.png",
        "./cubemap/ny.png",
        "./cubemap/pz.png",
        "./cubemap/nz.png",
    });
    
    for(auto& fb : framebuffers) {
        fb = Framebuffer(1024, 1024);
        framebuffer_samplers.push_back(fb.get_sampler());
    }
}

void draw_nocar() {
    programs.cubemap.use();
    objects.vehicle.get_translation() = camera.position;
    glDisable(GL_DEPTH_TEST);
    cubemap.bind();
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

void update() {
    handle_keys();
    
    for(int i = 0; i < 6; i++) {
        framebuffers[i].bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glDepthMask(true);
        glEnable(GL_DEPTH_TEST);
        draw_nocar();
        framebuffers[i].unbind();
    }
    glViewport(0, 0, width, height);
    
    draw_nocar();
    
    programs.car.use();
    programs.car.set("environment", (GLuint)0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, framebuffers[0].get_texture());
    LOG(framebuffers[0].get_texture());
    glBindSampler(0, framebuffers[0].get_sampler());
    objects.car.draw(programs.car, lights, camera);
}

static std::unordered_set<int> pressed_keys;

#define SPEED (0.01f)
#define ROTATION_SPEED (0.2f)
#define T_SPEED (1.0f / (180.0f / ROTATION_SPEED))

void handle_key(const int key) {
    const auto right = glm::normalize(glm::cross(camera.horizontal_direction, camera.up));
    
    if(key == GLFW_KEY_A) {
        camera.position -= right * SPEED;
    }
    if(key == GLFW_KEY_W) {
        camera.position += camera.horizontal_direction * SPEED;
    }
    if(key == GLFW_KEY_D) {
        camera.position += right * SPEED;
    }
    if(key == GLFW_KEY_S) {
        camera.position -= camera.horizontal_direction * SPEED;
    }
    if(key == GLFW_KEY_SPACE) {
        camera.position += glm::vec3{0.0f, SPEED, 0.0f};
    }
    if(key == GLFW_KEY_LEFT_SHIFT) {
        camera.position += glm::vec3{0.0f, -SPEED, 0.0f};
    }
    if(key == GLFW_KEY_LEFT) {
        camera.horizontal_direction = glm::rotate(glm::mat4(1.0f), glm::radians(ROTATION_SPEED), glm::vec3{0.0f, 1.0f, 0.0f}) * glm::vec4(camera.horizontal_direction, 0.0f);
    }
    if(key == GLFW_KEY_RIGHT) {
        camera.horizontal_direction = glm::rotate(glm::mat4(1.0f), glm::radians(-ROTATION_SPEED), glm::vec3{0.0f, 1.0f, 0.0f}) * glm::vec4(camera.horizontal_direction, 0.0f);
    }
    if(key == GLFW_KEY_UP) {
        camera.t = std::min(std::max(camera.t - T_SPEED, 0.0f), 1.0f);
    }
    if(key == GLFW_KEY_DOWN) {
        camera.t = std::max(std::min(camera.t + T_SPEED, 1.0f), 0.0f);
    }
    
    camera.direction = glm::mix(glm::quat{0.0f, glm::normalize(camera.horizontal_direction + glm::vec3{0.0f, 1.0f, 0.0f})}, glm::quat{0.0f, glm::normalize(camera.horizontal_direction + glm::vec3{0.0f, -1.0f, 0.0f})}, camera.t) * camera.horizontal_direction;
}

void handle_keys() {
    for(const int& key : pressed_keys) {
        handle_key(key);
    }
}

void key_event(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE and action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if(action == GLFW_PRESS) {
        pressed_keys.insert(key);
    } else if(action == GLFW_RELEASE) {
        pressed_keys.erase(pressed_keys.find(key));
    }
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
