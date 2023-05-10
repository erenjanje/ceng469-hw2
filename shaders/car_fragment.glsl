#version 460 core
in vec3 vertex_position;
flat in vec3 vertex_normal;
out vec4 frag_color;

uniform sampler2D environment;
uniform mat4 model;

void main() {
    frag_color = texture(environment, (inverse(model) * vec4(vertex_position, 1.0f)).xy);
}
