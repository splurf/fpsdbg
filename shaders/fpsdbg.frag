#version 460

in vec3 b_pos;
in vec3 b_norm;

out vec4 frag_color;

uniform bool has_norms;

void main() {
    vec3 light_color = vec3(0.2, 0.3, 1.0);

    if (has_norms) {
        vec3 light_pos = vec3(0.0, 0.0, 0.0);
        vec3 light_dir = normalize(light_pos - b_pos);
        vec3 norm = normalize(b_norm);
        float d = max(dot(light_dir, norm), 0.0);
        light_color *= d;
    }
    frag_color = vec4(light_color, 1.0);
}