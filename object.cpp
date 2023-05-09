#include "object.hpp"
#include <cstdint>
#include "utils.hpp"
#include <glm/ext.hpp>
#include <fstream>
#include <glm/ext/matrix_clip_space.hpp>
#include <sstream>
#include <array>

Object::Object() {
    vao = 0;
    index_count = 0;
}

Object::Object(const std::vector<float>& vertices, const std::vector<GLuint>& indices,
    glm::vec3 translation, glm::vec3 rotation, glm::vec3 ratios
) {
    std::vector<float> normals;
    normals.reserve(indices.size());
    
    for(std::size_t i = 0; i < indices.size()/3; i++) {
        const auto v1 = glm::vec3{vertices[3*indices[i]], vertices[3*indices[i] + 1], vertices[3*indices[i] + 2]};
        const auto v2 = glm::vec3{vertices[3*indices[i+1]], vertices[3*indices[i+2] + 1], vertices[3*indices[i+1] + 2]};
        const auto v3 = glm::vec3{vertices[3*indices[i+2]], vertices[3*indices[i+2] + 1], vertices[3*indices[i+2] + 2]};
        
        const auto e1 = v2 - v1;
        const auto e2 = v3 - v1;
        
        const auto n = glm::normalize(glm::cross(e1, e2));
        normals.push_back(n.x);
        normals.push_back(n.y);
        normals.push_back(n.z);
    }
    
    const auto vertex_size = vertices.size()*sizeof(float);
    const auto normal_size = normals.size()*sizeof(float);
    const auto index_size = indices.size()*sizeof(float);
    unsigned vertex_array, index_array;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vertex_array);
    glGenBuffers(1, &index_array);

    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array);
    glBufferData(GL_ARRAY_BUFFER, vertex_size+index_size, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_size, vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertex_size, index_size, normals.data());
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_array);        
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);

    index_count = indices.size();
    
    translate(translation);
    rotate(rotation);
    scale(ratios);
}

Object Object::with_texture(const std::vector<float>& vertices, const std::vector<GLuint>& indices, const std::vector<float>& texture_coordinates) {
    Object ret;
    std::vector<float> normals;
    normals.reserve(indices.size());
    
    for(std::size_t i = 0; i < indices.size()/3; i++) {
        const auto v1 = glm::vec3{vertices[3*indices[i]], vertices[3*indices[i] + 1], vertices[3*indices[i] + 2]};
        const auto v2 = glm::vec3{vertices[3*indices[i+1]], vertices[3*indices[i+2] + 1], vertices[3*indices[i+1] + 2]};
        const auto v3 = glm::vec3{vertices[3*indices[i+2]], vertices[3*indices[i+2] + 1], vertices[3*indices[i+2] + 2]};
        
        const auto e1 = v2 - v1;
        const auto e2 = v3 - v1;
        
        const auto n = glm::normalize(glm::cross(e1, e2));
        normals.push_back(n.x);
        normals.push_back(n.y);
        normals.push_back(n.z);
    }
    
    const auto vertex_size = vertices.size()*sizeof(float);
    const auto normal_size = normals.size()*sizeof(float);
    const auto index_size = indices.size()*sizeof(GLuint);
    const auto texture_size = texture_coordinates.size()*sizeof(float);
    unsigned vertex_array, index_array;

    glGenVertexArrays(1, &ret.vao);
    glGenBuffers(1, &vertex_array);
    glGenBuffers(1, &index_array);

    glBindVertexArray(ret.vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array);
    glBufferData(GL_ARRAY_BUFFER, vertex_size+index_size+texture_size, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_size, vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertex_size, normal_size, normals.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertex_size+index_size, texture_size, texture_coordinates.data());
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_array);        
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)vertex_size);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(vertex_size + normal_size));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);

    ret.index_count = indices.size();
    return std::move(ret);
}

Object Object::from_file(const std::string filename) {
    std::ifstream file(filename);
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<GLuint> indices;
    std::string tmp;
    for(std::string line; std::getline(file, line);) {
        std::stringstream s(line);
        if(line[0] == '#') { // comment
            continue;
        } else if(line[0] == 'v' and line[1] != 'n') { // vertex
            s >> tmp;
            float x, y, z;
            s >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        } else if(line[0] == 'v' and line[1] == 'n') { // normal
            s >> tmp;
            float x, y, z;
            s >> x >> y >> z;
            normals.push_back(x);
            normals.push_back(y);
            normals.push_back(z);
        } else if(line[0] == 'f') { // face
            s >> tmp;
            char c;
            std::array<GLuint, 3> vertex_index, normal_index;
            s >> vertex_index[0] >> c >> c >> normal_index[0];
            s >> vertex_index[1] >> c >> c >> normal_index[1];
            s >> vertex_index[2] >> c >> c >> normal_index[2];
            vertex_index[0] -= 1;
            vertex_index[1] -= 1;
            vertex_index[2] -= 1;
            indices.push_back(vertex_index[0]);
            indices.push_back(vertex_index[1]);
            indices.push_back(vertex_index[2]);
        }
    }
    return std::move(Object(vertices, indices));
}

void Object::draw(ShaderProgram& program, const std::vector<Light>& lights, const Camera& camera) {
    glm::mat4 model = glm::scale(glm::mat4(1.0f), scaling);
    model = glm::translate(model, translation);
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3{0.0f, 0.0f, 1.0f});
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3{0.0f, 1.0f, 0.0f});
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3{1.0f, 0.0f, 0.0f});

    program.use();
    program.set("model", model);
    program.set("view", glm::lookAt(camera.position, camera.position + camera.direction, camera.up));
    program.set("projection", glm::perspective(camera.fov, camera.aspect_ratio, camera.near, camera.far));
    program.set("lightpos", lights[0].position);
    program.set("lightintensity", lights[0].intensity);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, (void*)0);
    check_gl("Draw");
    glBindVertexArray(0);
}

Object& Object::translate(const glm::vec3 distances) {
    translation += distances;
    return *this;
}

Object& Object::rotate(const glm::vec3 angles) {
    rotation += angles;
    return *this;
}

Object& Object::scale(const glm::vec3 ratios) {
    scaling += ratios;
    return *this;
}

glm::vec3& Object::get_translation() {
    return translation;
}

glm::vec3& Object::get_rotation() {
    return rotation;
}

glm::vec3& Object::get_scale() {
    return scaling;
}
