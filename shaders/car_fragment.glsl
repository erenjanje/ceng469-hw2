#version 460 core
in vec3 vertex_position;
flat in vec3 vertex_normal;
out vec4 frag_color;

uniform sampler2D environment;

void main() {
    frag_color = texture(environment, vertex_normal.xy);
}
