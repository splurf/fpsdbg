#version 460

in vec3 a_pos;
in vec3 a_norm;

out vec3 b_pos;
out vec3 b_norm;

uniform mat4 projection;
uniform mat4 modelview;
uniform bool has_norms;

void main() {
    b_pos = (modelview * vec4(a_pos, 1.0)).xyz;

    if (has_norms) {
        mat3 norm_mat = transpose(inverse(mat3(modelview)));
        b_norm = normalize(norm_mat * a_norm);
    }
    gl_Position = projection * modelview * vec4(a_pos, 1.0);
}