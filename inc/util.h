/// Includes all IO operation and GLEW/GLFW methods and prototypes.
/// @file
/// @author Evan Schwartzentruber

#ifndef UTIL_H
#define UTIL_H

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


/// @brief Unsigned integer
typedef unsigned int uint;


/// @brief Simple object-struct, containing the information for drawing the geometry
typedef struct Object {
    uint vao, program, vertices_len, indices_len;
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
    mat4x4 v, m, p, t, r;
    vec3 eye, center, up, pos;
} camera;


/// @brief Basic shader struct
/// @param src shader content
/// @param type shader type
typedef struct Shader {
    const char *src;
    const int type;
} shader;


/// @brief Compile and link a shader from source
/// @param s GLuint identifier pointer
/// @param sh shader struct
/// @return status code of the function
int compile_shader(uint *s, const shader sh);


#endif // UTIL_H