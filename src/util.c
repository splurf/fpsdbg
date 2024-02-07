/// Includes all IO operation and GLEW/GLFW methods and prototypes.
/// @file
/// @author Evan Schwartzentruber

#include "util.h"


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