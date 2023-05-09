#version 460 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 pos;

out vec3 tex_coord;

void main() {
    tex_coord = pos;
    gl_Position = projection * view * model * vec4(pos, 1.0f);
}
