/// Includes all IO operation and GLEW/GLFW methods and prototypes.
/// @file
/// @author Evan Schwartzentruber

#include "util.h"


int compile_shader(uint *s, const shader sh) {
    *s = glCreateShader(sh.type); // new shader id
    glShaderSource(*s, 1, &sh.src, NULL); // link src of id to buf
    glCompileShader(*s); // compile shader

    // error handling
    int success;
    char infoLog[512];
    glGetShaderiv(*s, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(*s, 512, NULL, infoLog);
        error(infoLog);
        return 0;
    }
    return 1;
}