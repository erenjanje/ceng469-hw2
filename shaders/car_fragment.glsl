#version 460 core
in vec3 vertex_position;
in vec3 vertex_normal;
out vec4 frag_color;

uniform sampler2D environment[6];
uniform mat4 model;
uniform vec3 eyepos;

uniform int part;

uniform int render_type;

#define POSX 0
#define NEGX 1
#define POSY 2
#define NEGY 3
#define POSZ 4
#define NEGZ 5

void main() {
    vec3 ray = reflect(vertex_position - eyepos, normalize(vertex_normal));
    // ray = vertex_position - eyepos;
    vec3 abs_ray = abs(ray);
    int selected_face = POSX;
    vec2 uv;
    bool neg = false;
    if(abs_ray.x >= abs_ray.y && abs_ray.x >= abs_ray.z) {
        selected_face = POSX;
        if(ray.x < 0) {
            neg = true;
        }
        ray /= ray.x;
        ray = (ray/2.0) + vec3(0.5f, 0.5f, 0.5f);
        uv = ray.zy;
        if(neg) {
            uv.y = 1.0f-uv.y;
        }
    } else if(abs_ray.y >= abs_ray.x && abs_ray.y >= abs_ray.z) {
        selected_face = POSY;
        if(ray.y < 0) {
            neg = true;
        }
        ray /= ray.y;
        ray = (ray/2.0) + vec3(0.5f, 0.5f, 0.5f);
        uv = ray.xz;
        if(neg) {
            uv.y = 1.0f - uv.y;
        }
    } else if(abs_ray.z >= abs_ray.x && abs_ray.z >= abs_ray.y) {
        selected_face = POSZ;
        if(ray.z < 0) {
            neg = true;
        }
        ray /= ray.z;
        ray = (ray/2.0) + vec3(0.5f, 0.5f, 0.5f);
        uv = ray.xy;
        uv = vec2(1.0f, 1.0f) - uv;
        if(!neg) {
            uv.y = 1.0f - uv.y;
        }
    }
    if(neg) {
        selected_face += 1;
    }
    if(render_type == 0) {
        frag_color = texture(environment[selected_face], uv);
    } else if(render_type == 1) {
        frag_color = vec4(uv, 1.0f, 1.0f);
    } else {
        frag_color = texture(environment[selected_face], uv);
    }
    vec4 original_pos = inverse(model) * vec4(vertex_position, 1.0f);
    if(part == 0) {
        frag_color += vec4(0.0f, 0.2f, 0.0f, 0.0f);
    } else if(part == 1) {
        frag_color += vec4(0.0f, 0.0f, 0.2f, 0.0f);
    } else {
        frag_color = vec4(0.05f, 0.05f, 0.05f, 1.0f);
    }
}
