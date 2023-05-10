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
    -1.0f, -1.0f, -1.0f, // 8
     1.0f, -1.0f, -1.0f, // 9
     1.0f,  1.0f, -1.0f, // 10
    -1.0f,  1.0f, -1.0f, // 11
    -1.0f, -1.0f,  1.0f, // 12
     1.0f, -1.0f,  1.0f, // 13
     1.0f,  1.0f,  1.0f, // 14
    -1.0f,  1.0f,  1.0f, // 15
    -1.0f, -1.0f, -1.0f, // 16
     1.0f, -1.0f, -1.0f, // 17
     1.0f,  1.0f, -1.0f, // 18
    -1.0f,  1.0f, -1.0f, // 19
    -1.0f, -1.0f,  1.0f, // 20
     1.0f, -1.0f,  1.0f, // 21
     1.0f,  1.0f,  1.0f, // 22
    -1.0f,  1.0f,  1.0f, // 23
};

static const auto vehicle_normals = std::vector<float>{

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

bool car_creating = false;

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
    Object tires;
    
    void draw(ShaderProgram& shader, const std::vector<Light>& lights, const Camera& camera) {
        shader.set("part", (GLuint)0);
        body.draw(shader, lights, camera);
        shader.set("part", (GLuint)1);
        windows.draw(shader, lights, camera);
        shader.set("part", (GLuint)2);
        tires.draw(shader, lights, camera);
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

static const auto car_camera_offsets = std::array<glm::vec3, 4>{
    glm::vec3{ 0.0f, 1.0f, -3.0f}, // front
    glm::vec3{ 0.0f, 1.0f,  3.0f}, // back
    glm::vec3{ 3.0f, 1.0f,  0.0f}, // right
    glm::vec3{-3.0f, 1.0f,  0.0f}, // left
};

static const auto car_camera_directions = std::array<glm::vec3, 4>{
    glm::vec3{ 0.0f, 0.0f,  1.0f},
    glm::vec3{ 0.0f, 0.0f, -1.0f},
    glm::vec3{-1.0f, 0.0f,  0.0f},
    glm::vec3{ 1.0f, 0.0f,  0.0f},
};

static int car_camera = 0;

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
    objects.vehicle = Object(vehicle_vertices, vehicle_normals, vehicle_indices, glm::vec3{0.0f, 0.0f, -5.0f});
    objects.armadillo = Object::from_file("./assets/armadillo.obj").translate(glm::vec3{0.0f, 0.0f, -5.0f}).rotate(glm::vec3{0.0f, 180.0f, 0.0f});
    
    objects.car.body = Object::from_file("./assets/cybertruck_body.obj");
    objects.car.windows = Object::from_file("./assets/cybertruck_windows.obj");
    objects.car.tires = Object::from_file("./assets/cybertruck_tires.obj");
    
    // objects.car.body.get_translation() = glm::vec3{0.0f, 1.0f, -10.0f};
    // objects.car.windows.get_translation() = glm::vec3{0.0f, 1.0f, -10.0f};
    // objects.car.tires.get_translation() = glm::vec3{0.0f, 1.0f, -10.0f};

    objects.car.body.get_scale() = glm::vec3{0.2f, 0.2f, 0.2f};
    objects.car.windows.get_scale() = glm::vec3{0.2f, 0.2f, 0.2f};
    objects.car.tires.get_scale() = glm::vec3{0.2f, 0.2f, 0.2f};
    
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
        fb = Framebuffer(width, height);
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

static const std::array<glm::vec3, 6> camera_directions = {
    glm::vec3{ 1.0f,  0.0f,  0.0f},
    glm::vec3{-1.0f,  0.0f,  0.0f},
    glm::vec3{ 0.0f,  1.0f,  0.0f},
    glm::vec3{ 0.0f, -1.0f,  0.0f},
    glm::vec3{ 0.0f,  0.0f,  1.0f},
    glm::vec3{ 0.0f,  0.0f, -1.0f},
};

static const std::array<glm::vec3, 6> camera_ups = {
    glm::vec3{0.0f, 1.0f, 0.0f},
    glm::vec3{0.0f, 1.0f, 0.0f},
    glm::vec3{0.0f, 0.0f, 1.0f},
    glm::vec3{0.0f, 0.0f, 1.0f},
    glm::vec3{0.0f, 1.0f, 0.0f},
    glm::vec3{0.0f, 1.0f, 0.0f},
};

static GLuint render_type = 0;

static auto car_direction = 180.0f;
static auto car_speed = 0.0f;

#define CAR_SPEED_MULTIPLIER (0.01f)

void update() {
    handle_keys();
    const auto car_direction_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(car_direction), glm::vec3{0.0f, 1.0f, 0.0f});
    const auto car_direction_vector = glm::vec3(car_direction_matrix * glm::vec4{0.0f, 0.0f, 1.0f, 0.0f});
    objects.car.body.get_translation() += car_speed * car_direction_vector * CAR_SPEED_MULTIPLIER;
    objects.car.tires.get_translation() += car_speed * car_direction_vector * CAR_SPEED_MULTIPLIER;
    objects.car.windows.get_translation() += car_speed * car_direction_vector * CAR_SPEED_MULTIPLIER;
    
    objects.car.body.get_rotation().y = car_direction;
    objects.car.tires.get_rotation().y = car_direction;
    objects.car.windows.get_rotation().y = car_direction;
    
    Camera camera_backup = camera;
    camera.fov = 45.0f;
    camera.aspect_ratio = 1.0f;
    for(int i = 0; i < 6; i++) {
        framebuffers[i].bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glDepthMask(true);
        glEnable(GL_DEPTH_TEST);

        camera.position = objects.car.body.get_translation();
        camera.direction = camera_directions[i];
        camera.up = camera_ups[i];

        draw_nocar();
        glTextureBarrier();
        framebuffers[i].unbind();
    }
    
    auto camera_direction = car_camera_directions[car_camera];
    camera_direction = glm::vec3(car_direction_matrix * glm::vec4(camera_direction, 0.0f));
    camera.direction = camera_direction;
    
    auto camera_position = car_camera_offsets[car_camera];
    camera_position = glm::vec3(car_direction_matrix * glm::vec4(camera_position, 1.0f));
    camera_position = camera_position + objects.car.body.get_translation()*objects.car.body.get_scale();
    camera.position = camera_position;
    
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    draw_nocar();
    
    glEnable(GL_DEPTH_TEST);
    programs.car.use();
    programs.car.set("environment", std::vector<GLuint>{1,2,3,4,5,6});
    programs.car.set("eyepos", camera.position);
    programs.car.set("render_type", render_type);

    for(int i = 0; i < 6; i++) {
        glActiveTexture(GL_TEXTURE0 + 1+i);
        glBindTexture(GL_TEXTURE_2D, framebuffers[i].get_texture());
        glBindSampler(i+1, framebuffers[i].get_sampler());
    }
    programs.car.set("part", (GLuint)0);
    objects.car.body.draw(programs.car, lights, camera);
    programs.car.set("part", (GLuint)1);
    objects.car.windows.draw(programs.car, lights, camera);
    programs.car.set("part", (GLuint)2);
    objects.car.tires.draw(programs.car, lights, camera);
}

static std::unordered_set<int> pressed_keys;

#define SPEED (0.01f)
#define ROTATION_SPEED (0.2f)
#define T_SPEED (1.0f / (180.0f / ROTATION_SPEED))

void handle_key(const int key) {
    const auto right = glm::normalize(glm::cross(camera.horizontal_direction, camera.up));
    
    if(key == GLFW_KEY_A) {
        car_direction += ROTATION_SPEED;
    }
    if(key == GLFW_KEY_W) {
        car_speed += SPEED;
    }
    if(key == GLFW_KEY_D) {
        car_direction -= ROTATION_SPEED;
    }
    if(key == GLFW_KEY_S) {
        car_speed -= SPEED;
    }
    if(key == GLFW_KEY_SPACE) {
        // camera.position += glm::vec3{0.0f, SPEED, 0.0f};
    }
    if(key == GLFW_KEY_LEFT_SHIFT) {
        // camera.position += glm::vec3{0.0f, -SPEED, 0.0f};
    }
    if(key == GLFW_KEY_LEFT) {
        // camera.horizontal_direction = glm::rotate(glm::mat4(1.0f), glm::radians(ROTATION_SPEED), glm::vec3{0.0f, 1.0f, 0.0f}) * glm::vec4(camera.horizontal_direction, 0.0f);
    }
    if(key == GLFW_KEY_RIGHT) {
        // camera.horizontal_direction = glm::rotate(glm::mat4(1.0f), glm::radians(-ROTATION_SPEED), glm::vec3{0.0f, 1.0f, 0.0f}) * glm::vec4(camera.horizontal_direction, 0.0f);
    }
    if(key == GLFW_KEY_UP) {
        // camera.t = std::min(std::max(camera.t - T_SPEED, 0.0f), 1.0f);
    }
    if(key == GLFW_KEY_DOWN) {
        // camera.t = std::max(std::min(camera.t + T_SPEED, 1.0f), 0.0f);
    }
    
    // camera.direction = glm::mix(glm::quat{0.0f, glm::normalize(camera.horizontal_direction + glm::vec3{0.0f, 1.0f, 0.0f})}, glm::quat{0.0f, glm::normalize(camera.horizontal_direction + glm::vec3{0.0f, -1.0f, 0.0f})}, camera.t) * camera.horizontal_direction;
}

void handle_keys() {
    for(const int& key : pressed_keys) {
        handle_key(key);
    }
}

void key_event(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE and action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return;
    } else if(key == GLFW_KEY_Q and action == GLFW_PRESS) {
        car_camera = 3;
        return;
    } else if(key == GLFW_KEY_E and action == GLFW_PRESS) {
        car_camera = 2;
        return;
    } else if(key == GLFW_KEY_R and action == GLFW_PRESS) {
        car_camera = 0;
        return;
    } else if(key == GLFW_KEY_T and action == GLFW_PRESS) {
        car_camera = 1;
        return;
    } else if(key == GLFW_KEY_SPACE and action == GLFW_PRESS) {
        car_speed = 0.0f;
    }
    if(key == GLFW_KEY_Q) {
        return;
    }
    if(action == GLFW_PRESS) {
        pressed_keys.insert(key);
    } else if(action == GLFW_RELEASE) {
        const auto found = pressed_keys.find(key);
        if(found != pressed_keys.end()) {
            pressed_keys.erase(pressed_keys.find(key));
        }
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
