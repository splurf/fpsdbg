/* Contains convenience methods and such for handling objects and other tedious works. */

/**
 * @file
 * @author Evan Schwartzentruber
*/

#include "util.h"
#include "linmath.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// globally established window dimensions
static GLuint WIDTH, HEIGHT;

/** Simple object-struct, containing the information for drawing the geometry */
typedef struct Object {
    GLuint vao, program, vertices_len, indices_len;
    GLenum mode;
    GLboolean has_norms, has_ebo;
} obj;

/**
 * Encapsulation of all objects.
 *
 * `objects` : array of objects
 * `objects_len` : number of objects
 */
typedef struct World {
    obj *objects;
    GLuint objects_len;
} world;

/** Basic camera struct */
typedef struct Camera {
    vec3 eye, center, up;
} camera;

/** Modify viewport to current window size */
void framebuffer_size_callback(GLFWwindow *window, int w, int h) {
    WIDTH = w, HEIGHT = h;
    glViewport(0, 0, w, h);
}

// Calculate the normals for the provided vertex data
void calc_norm(const GLuint n, const vec3 *vertices, vec3 *normals) {
    /**
     * `i` => index of vertex (a)
     * `j` => index of vertex (b)
     * `k` => index of vertex (c)
     */
    for (GLuint i = 0; i < n; i += 3)
    {
        const GLuint j = i + 1;
        const GLuint k = j + 1;

        // the neighboring edge
        const float *a = vertices[i];

        // edge (u)
        const float *b = vertices[j];

        // edge (v)
        const float *c = vertices[k];

        // differences between edges
        vec3 dif_b, dif_c;
        vec3_sub(dif_b, b, a);
        vec3_sub(dif_c, c, a);

        // cross product then unit length
        vec3 prod, norm;
        vec3_mul_cross(prod, dif_b, dif_c);
        vec3_norm(norm, prod);

        // set each vertex to the unit vector
        for (int l = 0; l < 3; l++)
        {
            normals[i][l] = norm[l];
            normals[j][l] = norm[l];
            normals[k][l] = norm[l];
        }
    }
}

/** Create an object and automatically store it into the world container (this is unchecked, assumes there's enough space allocated) */
void create_object(world *w, const GLuint program, const GLuint n, const GLuint m, const GLfloat *vertices, const GLuint *indices, const GLboolean has_norms, const GLenum usage, const GLenum mode) {
    GLuint vao, vbo;
    GLboolean has_ebo = m > 0;

    // creates and bind Vertex Array Object (VAO)
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // create and bind Vertex Buffer Object (VBO)
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, n * 4, vertices, usage);

    if (has_ebo) {
        GLuint ebo;
        // create and bind Elements Buffer Object (EBO)
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m * 4, indices, usage);
    }

    // enable `a_pos` vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    if (has_norms) {
        // total number of triangles
        const GLuint n_of_tri = n / 3;

        // calculate the normals of the geometry
        vec3 normals[n_of_tri];
        calc_norm(n_of_tri, (vec3 *)vertices, normals);

        glBindBuffer(GL_NORMAL_ARRAY, vbo);
        glBufferData(GL_NORMAL_ARRAY, n * 4, normals, usage);

        // enable `a_norm` vertex attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
    }

    // unbind everything
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    w->objects[w->objects_len++] = (obj){vao, program, n, m, mode, has_norms, has_ebo};
}

/** Read and compile a shader file from source */
GLuint shader_from_file(char *file_name, const int shader_type) {
    const char *buf = read_file(file_name);
    GLuint s = glCreateShader(shader_type);
    glShaderSource(s, 1, &buf, NULL);
    glCompileShader(s);

    int success;
    char infoLog[512];
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(s, 512, NULL, infoLog);
        error(infoLog);
        exit(EXIT_FAILURE);
    }
    return s;
}

/** Convenience method for initializing the `GLFW` and `GLEW` libraries as well as a new and simple window */
GLFWwindow *init() {
    glfwSetErrorCallback(_error);

    if (!glfwInit()) {
        error("GLFW failed to initiate.\n");
        exit(EXIT_FAILURE);
    }
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    if (!monitor) {
        error("Failed to get primary monitor.");
        exit(EXIT_FAILURE);
    }
    const GLFWvidmode *video = glfwGetVideoMode(monitor);

    if (!video) {
        error("Failed to get video mode of monitor");
        exit(EXIT_FAILURE);
    }
    // for a consistent window size
    const int w = video->width / 2;
    const int h = video->height / 2;

    // using GLSL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // 8 samples of anti-aliasing
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow *window = glfwCreateWindow(w, h, "fpsdbg", NULL, NULL);

    if (!window) {
        error("GLFW window failed to initialize.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        error("GLEW failed to initiate.\n");
        exit(EXIT_FAILURE);
    }
    // enable and setup some simple features
    glEnable(GL_MULTISAMPLE);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // automatically handle framebuffer_size changes
    framebuffer_size_callback(window, w, h);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}