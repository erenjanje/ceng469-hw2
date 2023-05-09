#version 460 core
out vec4 frag_color;

in vec2 vertex_tex_coord;

uniform sampler2D the_texture;

void main() {
    frag_color = texture(the_texture, vertex_tex_coord);
}
