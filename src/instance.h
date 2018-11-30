#ifndef INSTANCE_H
#define INSTANCE_H

#define GLFW_INCLUDE_VULKAN

#include <windows.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

void createInstance(unsigned int enableValidation, VkInstance *instance);
void createSurface(VkInstance instance, VkSurfaceKHR *surface, GLFWwindow *window);

#endif