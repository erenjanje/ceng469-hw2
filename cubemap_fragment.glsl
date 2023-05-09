#version 460 core

uniform samplerCube cube_sampler;

in vec3 tex_coord;
out vec4 frag_color;

void main() {
    vec3 coord = tex_coord;
    coord.x *= -1.0;
    frag_color = texture(cube_sampler, coord);
}
