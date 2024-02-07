/// Contains convenience methods and such for handling objects and other tedious works.
/// @file
/// @author Evan Schwartzentruber

#ifndef FPSDBG_H
#define FPSDBG_H

#include "util.h"

// window dimensions
extern uint WIDTH, HEIGHT;
extern float ASPECT;

// main camera
extern camera cam;

/// @brief Update the camera's matrices
void upt_cam();

/// @brief Modify viewport to current window size
/// @param window initialized GLFW window
/// @param w the new width
/// @param h the new height
void framebuffer_size_callback(GLFWwindow *window, const int w, const int h);

/// @brief Update camera position on scroll callback
/// @param window initialized GLFW window
/// @param xoffset scroll xoffset value
/// @param yoffset scroll yoffset value
void scroll_callback(GLFWwindow *window, const double xoffset, const double yoffset);

/// @brief Calculate the normals for the provided vertex data
/// @param n number of triangles
/// @param vertices vertices array
/// @param normals normals array
void calc_norm(const uint n, const vec3 vertices[], vec3 normals[]);

/// @brief Create an object and automatically store it into the world container (this is unchecked, assumes there's enough space allocated)
/// @param wd world pointer
/// @param program current program
/// @param n size of vertices array
/// @param m size of indices array
/// @param vertices vertices array
/// @param indices indices array
/// @param usage type of usage
/// @param mode rendering mode
/// @return GLuint identifier
uint create_object(world *wd, const uint program, const uint n, const uint m, const float *vertices, const uint *indices, const GLenum usage, const GLenum mode);

/// @brief Create a rectangular-prism based on the provided dimensions
/// @param wd world pointer
/// @param program current program
/// @param pos position of geometry
/// @param dim dimensions of geometry
/// @return GLuint identifier
uint create_rect(world *wd, const uint program, const vec3 pos, const vec3 dim);

/// @brief Convenience method for initializing the `GLFW` and `GLEW` libraries as well as a new and simple window
/// @return newly initialized GLFW window
GLFWwindow *init();

#endif