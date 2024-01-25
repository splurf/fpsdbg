/// Includes all IO operation and GLEW/GLFW methods and prototypes.
/// @file
/// @author Evan Schwartzentruber

#include "err.h"
#include "linmath.h"
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Define commonly-used constants
#ifndef M_PI
    #define M_PI 3.141592
#endif
#define M_TAU 6.283185

/// @brief Return a buffer of the contents of the file at the provided path
/// @param path path of file
/// @return the contents of the file
char *read_file(char *path) {
    FILE *f = fopen(path, "r");

    if (!f) {
        error("File not found");
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    size_t len = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc((len + 1) * sizeof(char));
    size_t bytes = fread(buffer, 1, len, f);
    buffer[len] = '\0';

    return bytes > 0 ? buffer : NULL;
}

/// @brief Read and compile a shader file from source
/// @param file_name path of file
/// @param shader_type type of shader
/// @return cooresponding unsigned integer value of shader
GLuint shader_from_file(char *file_name, const int shader_type) {
    const char *buf = read_file(file_name); // read contents of file
    GLuint s = glCreateShader(shader_type); // new shader id
    glShaderSource(s, 1, &buf, NULL); // link src of id to buf
    glCompileShader(s); // compile shader

    {
        // error handling
        int success;
        char infoLog[512];
        glGetShaderiv(s, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(s, 512, NULL, infoLog);
            error(infoLog);
            exit(EXIT_FAILURE);
        }
    }

    return s;
}

/// @brief Unsigned integer
typedef unsigned int uint;

/// @brief Simple object-struct, containing the information for drawing the geometry
typedef struct Object {
    GLuint vao, program, vertices_len, indices_len;
    GLenum mode;
    GLboolean has_ebo;
} obj;

/// @brief Encapsulation of all objects.
/// @param objects array of objects
/// @param objects_len number of objects
typedef struct World {
    obj *objects;
    uint objects_len;
} world;

/// @brief Basic camera struct
/// @param eye eye attribute
/// @param center center attribute
/// @param up up attribute
/// @param pos position
/// @param v view matrix
/// @param m modelview matrix
/// @param p projection matrix
/// @param t translation matrix
/// @param r rotation matrix
typedef struct Camera {
    vec3 eye, center, up, pos;
    mat4x4 v, m, p, t, r;
} camera;