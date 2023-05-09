#version 460 core
in vec4 vertex_color;
out vec4 frag_color;

void main() {
    frag_color = vertex_color;
//     frag_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
