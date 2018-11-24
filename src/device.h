#ifndef DEVICE_H
#define DEVICE_H

#define GLFW_INCLUDE_VULKAN

#include <windows.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

typedef struct QueueFamilyIndices {
	int graphicsFamily;
	int presentFamily;
	int (*isComplete)(int, int);
} QueueFamilyIndices;

//typedef struct vkGraphicsDevice {
//	VkDevice device;
//	VkPhysicalDevice physicalDevice;
//	VkQueue graphicsQueue;
//	VkQueue presentQueue;
//} vkGraphicsDevice;

typedef struct swapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	VkSurfaceFormatKHR *formats;
	VkPresentModeKHR *presentModes;
} swapChainSupportDetails;

QueueFamilyIndices *findQueueFamilies(VkPhysicalDevice vkDevice, VkSurfaceKHR surface);
swapChainSupportDetails querySwapChainSupport(VkPhysicalDevice vkDevice, VkSurfaceKHR surface, unsigned int *formatCount, unsigned int *presentModeCount);
void pickPhysicalDevice(VkPhysicalDevice *physicalDevice, VkInstance instance, VkSurfaceKHR surface);
void createLogicalDevice(VkDevice *device, VkQueue *graphicsQueue, VkQueue *presentQueue, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

#endif