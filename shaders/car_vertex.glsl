#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightpos;
uniform vec3 lightintensity;
uniform vec3 eyepos;

out vec3 vertex_position;
out vec3 vertex_normal;

void main() {
    vertex_normal = (inverse(model) * vec4(normal, 0.0f)).xyz;
    vertex_position = (model * vec4(pos, 1)).xyz;
    gl_Position = projection * view * vec4(vertex_position, 1.0f);
}
