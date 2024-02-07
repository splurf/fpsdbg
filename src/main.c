#include "fpsdbg.h"


const shader SHADER_VERT = {"                             \n\
#version 460                                              \n\
                                                          \n\
layout(location = 0) in vec3 a_pos;                       \n\
layout(location = 1) in vec3 a_norm;                      \n\
                                                          \n\
out vec3 b_pos; // modified position                      \n\
out vec3 b_norm; // modified normals                      \n\
                                                          \n\
layout(location = 0) uniform mat4 modelview;              \n\
layout(location = 1) uniform mat4 projection;             \n\
                                                          \n\
void main() {                                             \n\
    vec4 pos = vec4(a_pos, 1.0);                          \n\
    b_pos = (modelview * pos).xyz;                        \n\
    mat3 norm_mat = transpose(inverse(mat3(modelview)));  \n\
    b_norm = normalize(norm_mat * a_norm);                \n\
    gl_Position = projection * modelview * pos;           \n\
}                                                         \n\
", GL_VERTEX_SHADER
                           };

const shader SHADER_FRAG = {"                          \n\
#version 460                                           \n\
                                                       \n\
in vec3 b_pos;                                         \n\
in vec3 b_norm;                                        \n\
                                                       \n\
out vec4 frag_color;                                   \n\
                                                       \n\
void main() {                                          \n\
    vec3 light_pos = vec3(0.0, 0.0, 0.0);              \n\
    vec3 light_dir = normalize(light_pos - b_pos);     \n\
    vec3 norm = normalize(b_norm);                     \n\
    float diffuse = max(dot(light_dir, norm), 0.0);    \n\
    vec3 light_color = vec3(0.2, 0.3, 1.0) * diffuse;  \n\
    frag_color = vec4(light_color, 1.0);               \n\
}                                                      \n\
", GL_FRAGMENT_SHADER
                           };

/// Key callback
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // ignore key releases
    if (action == GLFW_RELEASE)
        return;

    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_LEFT:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case GLFW_KEY_RIGHT:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case GLFW_KEY_W:
            cam.pos[2] -= 0.2;
            break;
        case GLFW_KEY_A:
            cam.pos[0] -= 0.2;
            break;
        case GLFW_KEY_S:
            cam.pos[2] += 0.2;
            break;
        case GLFW_KEY_D:
            cam.pos[0] += 0.2;
            break;
        case GLFW_KEY_SPACE:
            cam.pos[1] -= 0.2;
            break;
        case GLFW_KEY_LEFT_SHIFT:
            cam.pos[1] += 0.2;
            break;
    }
    // in case the camera's position has changed
    upt_cam();
}

/// Handle drawing everything to the window
void display(GLFWwindow *window, world w) {
    // clear the screen
    glClearColor(0.4, 0.4, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw each object
    for (uint i = 0; i < w.objects_len; i++) {
        const obj o = w.objects[i]; // the current object

        // init rotation matrix
        mat4x4_identity(cam.r);
        mat4x4_rotate_Y(cam.r, cam.r, glfwGetTime());

        // Instead of calling `upt_cam` to fully update the camera,
        // clone only the camera's modelview matrix as it's the
        // only matrix being modified; everything else stays the
        // same, so there's no need to recalculate their values
        mat4x4 m;
        mat4x4_dup(m, cam.m);

        // multiply modelview with rotation matrix
        mat4x4_mul(m, m, cam.r);

        // use the correct program
        glUseProgram(o.program);

        // init uniforms
        glUniformMatrix4fv(0, 1, GL_FALSE, (float *)m); // modelview
        glUniformMatrix4fv(1, 1, GL_FALSE, (float *)cam.p); // projection

        // bind and draw object
        glBindVertexArray(o.vao);

        // draw the object
        if (o.has_ebo)
            glDrawElements(o.mode, o.indices_len, GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(o.mode, 0, o.vertices_len);
    }
}

int main(int argc, char **argv) {
    // init GLFW and GLEW and window
    GLFWwindow *window = init();

    // shader files (vertex and fragment)
    uint vs, fs;
    if (!(compile_shader(&vs, SHADER_VERT) && compile_shader(&fs, SHADER_FRAG))) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // init program with shaders
    const uint program = glCreateProgram();
    glAttachShader(program, vs); // link vertex shader
    glAttachShader(program, fs); // link fragment shader
    glLinkProgram(program); // link program
    glDeleteShader(vs); // no longer needed
    glDeleteShader(fs); // no longer needed

    // assign callbacks
    glfwSetKeyCallback(window, key_callback);

    // init world container (for managing all objects)
    world wd = (world) {
        .objects = (obj *)malloc(sizeof(obj)),
        .objects_len = 0
    };

    create_rect(&wd, program, (vec3) {
        -0.5, -0.5, -0.5
    }, (vec3) {
        1, 1, 1
    });

    while (!glfwWindowShouldClose(window)) {
        display(window, wd);
        // update other events like input handling
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
    }

    // clean up
    free(wd.objects);
    glfwDestroyWindow(window);
    glfwTerminate();
}