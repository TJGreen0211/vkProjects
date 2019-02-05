#ifndef LOADOBJECT_H
#define LOADOBJECT_H

#define GLFW_INCLUDE_VULKAN

#include <windows.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <linearAlg.h>
#include <mman.h>

void loadObject(const char *filepath);

#endif