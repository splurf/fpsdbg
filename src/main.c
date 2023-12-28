#include "fpsdbg.h"
#include <stdbool.h>
#include <time.h>

const camera cam = {
    .eye    = { 0.0,  0.0, -2.0 },
    .center = { 0.0,  0.0,  0.0 },
    .up     = { 0.0,  1.0,  0.0 }
};

/** Key callback */
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // disregard the release of keys
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
    }
}

/** Handle drawing everything to the window */
void display(GLFWwindow *window, world w) {
    // clear the screen
    glClearColor(0.4, 0.4, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw each object
    for (int i = 0; i < w.objects_len; i++) {
        const obj o = w.objects[i]; // the current object

        /**
         * `v` : view
         * `r` : rotate
         * `m` : modelview
         * `p` : projection
         */
        mat4x4 v, r, m, p;

        // init view matrix
        mat4x4_look_at(v, cam.eye, cam.center, cam.up);

        // init rotation matrix
        mat4x4_identity(r);
        mat4x4_rotate_Y(r, r, glfwGetTime());

        // init modelview matrix (view * rotate)
        mat4x4_identity(m);
        mat4x4_mul(m, m, v);
        mat4x4_mul(m, m, r);

        // init projection matrix
        mat4x4_perspective(p, 0.8, (float)WIDTH / (float)HEIGHT, 0.01, 60.0);

        // use the correct program
        glUseProgram(o.program);

        // init uniforms
        glUniformMatrix4fv(0, 1, GL_FALSE, (float *)p); // projection
        glUniformMatrix4fv(1, 1, GL_FALSE, (float *)m); // modelview
        glUniform1ui(2, o.has_norms);                   // has_norms

        // bind and draw object
        glBindVertexArray(o.vao);

        // draw the object
        if (o.has_ebo)
            glDrawElements(o.mode, o.indices_len, GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(o.mode, 0, o.vertices_len);
    }
    glUseProgram(0);
}

int main(int argc, char **argv) {
    GLFWwindow *window = init(); // new window

    // shader files (vertex and fragment)
    const GLuint vs = shader_from_file("shaders/fpsdbg.vert", GL_VERTEX_SHADER);
    const GLuint fs = shader_from_file("shaders/fpsdbg.frag", GL_FRAGMENT_SHADER);

    // init program with shaders
    const GLuint program = glCreateProgram();
    glAttachShader(program, vs); // link vertex shader
    glAttachShader(program, fs); // link fragment shader
    glLinkProgram(program); // link program
    glDeleteShader(vs); // no longer needed
    glDeleteShader(fs); // no longer needed

    // assign callbacks
    glfwSetKeyCallback(window, key_callback);

    // init world container (for managing all objects)
    world w = (world){
        .objects = (obj *)malloc(sizeof(obj)),
        .objects_len = 0};

    // position of the object
    const float x = -0.5;
    const float y = -0.5;
    const float z = -0.5;

    // size of the object
    const float wx = 1.0;
    const float vy = 1.0;
    const float hz = 1.0;

    // temp
    const float x_w = x + wx;
    const float y_v = y + vy;
    const float z_h = z + hz;

    // vertex positions for a cube
    const GLfloat vertices[] = {
        x,   y,   z,   // (0, 0, 0) [0]
        x,   y,   z_h, // (0, 0, 1) [1]
        x,   y_v, z,   // (0, 1, 0) [2]
        x,   y_v, z_h, // (0, 1, 1) [3]
        x_w, y,   z,   // (1, 0, 0) [4]
        x_w, y,   z_h, // (1, 0, 1) [5]
        x_w, y_v, z,   // (1, 1, 0) [6]
        x_w, y_v, z_h, // (1, 1, 1) [7]
    };

    // form the cube with all counter-clockwise triangles
    const GLuint indices[] = {
        // FRONT
        1, 5, 7,
        7, 3, 1,

        // RIGHT
        5, 4, 6,
        6, 7, 5,

        // BACK
        2, 6, 4,
        4, 0, 2,

        // LEFT
        0, 1, 3,
        3, 2, 0,

        // BOTTOM
        0, 4, 5,
        5, 1, 0,

        // TOP
        3, 7, 6,
        6, 2, 3
    };

    // create the cube (with normals)
    create_object(&w, program, 24, 36,
                  vertices,
                  indices,
                  true,
                  GL_DYNAMIC_DRAW,
                  GL_TRIANGLES);

    while (!glfwWindowShouldClose(window)) {
        display(window, w);
        // update other events like input handling
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}