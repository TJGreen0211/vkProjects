#ifndef CAMERA_H
#define CAMERA_H
#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <windows.h>
#include <GLFW/glfw3.h>

#include "linearAlg.h"

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

vec3 getCamera();
vec4 getCameraPosition(mat4 position);
mat4 getViewMatrix();
mat4 getViewPosition();
mat4 getViewRotation();

void updateCameraVectors();
void processKeyboard(enum Camera_Movement direction, double deltaTime , double deltaSpeed);
void processMouseMovement(double xpos, double ypos, int resetFlag);
double processMouseScroll(double yoffset);

#endif
