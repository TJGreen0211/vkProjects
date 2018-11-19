#ifndef WINDOW_H
#define WINDOW_H
#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <windows.h>
#include <GLFW/glfw3.h>

GLFWwindow *initWindow();
int getWindowWidth();
int getWindowHeight();

#endif