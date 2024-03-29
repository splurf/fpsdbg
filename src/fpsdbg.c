#include "fpsdbg.h"

uint WIDTH, HEIGHT;
float ASPECT;

camera cam = {
    .eye = {0.0, 0.0, -2.0},
    .center = {0.0, 0.0, 0.0},
    .up = {0.0, 1.0, 0.0},
    .pos = {0.0, 0.0, 0.0}
};

void upt_cam() {
    // init view matrix
    mat4x4_look_at(cam.v, cam.eye, cam.center, cam.up);

    // init projection matrix
    mat4x4_perspective(cam.p, 0.8, ASPECT, 0.001, 1000.0);

    // init modelview matrix (model * view * translate * ...)
    mat4x4_identity(cam.m);
    mat4x4_mul(cam.m, cam.m, cam.v);

    mat4x4_identity(cam.t);
    mat4x4_translate(cam.t, cam.pos[0], cam.pos[1], cam.pos[2]);
    mat4x4_mul(cam.m, cam.m, cam.t);
}

void framebuffer_size_callback(GLFWwindow *window, const int w, const int h) {
    // update dimensions
    WIDTH = w, HEIGHT = h;

    // update aspect ratio based on the longer side
    ASPECT = (w > h) ? (float)w / h : (float)h / w;

    // reset viewport
    glViewport(0, 0, w, h);

    // update camera
    upt_cam();
}

void scroll_callback(GLFWwindow *window, const double xoffset, const double yoffset) {
    cam.pos[2] -= yoffset; // adjust z-pos
    upt_cam(); // update camera
}

void calc_norm(const uint n, const vec3 vertices[], vec3 normals[]) {
    /// @param i index of vertex (a)
    /// @param j index of vertex (b)
    /// @param k index of vertex (c)
    /// @param l temp index variable
    uint i, j, k, l;

    /// @param a neighboring edge
    /// @param b edge (u)
    /// @param c edge (v)
    const float *a, *b, *c;

    /// @param dif_b magnitude between edges `a` and `b`
    /// @param dif_c magnitude between edges `a` and `c`
    /// @param prod cross product between vectors `dif_b` and `dif_c`
    /// @param norm normalization of the cross product
    vec3 dif_b, dif_c, prod, norm;

    for (i = 0; i < n; i += 3) {
        j = i + 1;
        k = j + 1;

        a = vertices[i],
        b = vertices[j],
        c = vertices[k];

        // differences between edges
        vec3_sub(dif_b, b, a);
        vec3_sub(dif_c, c, a);

        // cross product then unit length
        vec3_mul_cross(prod, dif_b, dif_c);
        vec3_norm(norm, prod);

        // set each vertex to the unit vector
        for (l = 0; l < 3; l++) {
            normals[i][l] = norm[l];
            normals[j][l] = norm[l];
            normals[k][l] = norm[l];
        }
    }
}

uint create_object(world *wd, const uint program, const uint n, const uint m, const float *vertices, const uint *indices, const GLenum usage, const GLenum mode) {
    uint vao, vbo;
    GLboolean has_ebo = m > 0;

    {
        // init buffers and `a_pos` attribute
        // creates and bind Vertex Array Object (VAO)
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // create and bind Vertex Buffer Object (VBO)
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, n * sizeof(float), vertices, usage);

        // create and bind Elements Buffer Object (EBO)
        if (has_ebo) {
            uint ebo;
            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m * sizeof(float), indices, usage);
        }

        // enable `a_pos` vertex attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
    }

    {
        // init `a_norm` attribute
        // total number of triangles
        const uint n_of_tri = n / 3;

        // calculate the normals of the geometry
        vec3 normals[n_of_tri];
        calc_norm(n_of_tri, (vec3 *)vertices, normals);

        glBindBuffer(GL_NORMAL_ARRAY, vbo);
        glBufferData(GL_NORMAL_ARRAY, n * sizeof(float), normals, usage);

        // enable `a_norm` vertex attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
    }

    // unbind everything
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // current number of objects
    const uint i = wd->objects_len;

    // assign next object
    wd->objects[i] = (obj) {
        vao, program, n, m, mode, has_ebo
    };

    // increment size
    wd->objects_len += 1;

    return i;
}

uint create_rect(world *wd, const uint program, const vec3 pos, const vec3 dim) {
    const float
    x = pos[0], // x-pos
    y = pos[1], // y-pos
    z = pos[2], // z-pos
    w = dim[0], // width
    h = dim[1], // height
    l = dim[2]; // length

    // temp values
    const float xw = x + w;
    const float yh = y + h;
    const float zl = z + l;

    // vertex positions for a cube
    const float vertices[] = {
        x, y, z, // (0, 0, 0) [0]
        x, y, zl, // (0, 0, 1) [1]
        x, yh, z, // (0, 1, 0) [2]
        x, yh, zl, // (0, 1, 1) [3]
        xw, y, z, // (1, 0, 0) [4]
        xw, y, zl, // (1, 0, 1) [5]
        xw, yh, z, // (1, 1, 0) [6]
        xw, yh, zl, // (1, 1, 1) [7]
    };

    // form the cube with all counter-clockwise triangles
    const uint indices[] = {
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
    return create_object(wd, program, 24, 36,
                         vertices,
                         indices,
                         GL_STATIC_DRAW,
                         GL_TRIANGLES);
}

GLFWwindow *init() {
    glfwSetErrorCallback(_error);

    // init GLFW
    if (!glfwInit()) {
        error("Failed to initialize GLFW.");
        exit(EXIT_FAILURE);
    }

    // retrieve primary monitor
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        error("Failed to get primary monitor.");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // retrive video mode of primary monitor
    const GLFWvidmode *video = glfwGetVideoMode(monitor);
    if (!video) {
        error("Failed to get video mode.");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // initial window dimensions
    const int w = video->width / 2;
    const int h = video->height / 2;

    // use GLSL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // 8 samples of anti-aliasing
    glfwWindowHint(GLFW_SAMPLES, 8);

    // init GLFW window
    GLFWwindow *window = glfwCreateWindow(w, h, "fpsdbg", NULL, NULL);
    if (!window) {
        error("Failed to initialize GLFW window.");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // init GLEW
    if (glewInit() != GLEW_OK) {
        error("Failed to initialize GLEW.");
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // simple default config
    glEnable(GL_MULTISAMPLE);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // automatically handle framebuffer_size changes
    framebuffer_size_callback(window, w, h);

    // init buffer size callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // init scroll callback
    glfwSetScrollCallback(window, scroll_callback);

    // set the GLFW time
    glfwSetTime(0);

    return window;
}