#version 460

layout(location = 0) in vec3 a_pos; // initial position
layout(location = 1) in vec3 a_norm; // initial normals

out vec3 b_pos; // modified position
out vec3 b_norm; // modified normals

layout(location = 0) uniform mat4 modelview; // modelview matrix
layout(location = 1) uniform mat4 projection; // projection matrix

void main() {
    vec4 pos = vec4(a_pos, 1.0);
    b_pos = (modelview * pos).xyz;
    mat3 norm_mat = transpose(inverse(mat3(modelview)));
    b_norm = normalize(norm_mat * a_norm);
    gl_Position = projection * modelview * pos;
}