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

typedef struct vkGraphics {
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;

	VkCommandPool commandPool;
	VkImageView textureImageView;
	VkSampler textureSampler;
	VkDescriptorSetLayout descriptorSetLayout;
} vkGraphics;

typedef struct swapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	VkSurfaceFormatKHR *formats;
	VkPresentModeKHR *presentModes;
} swapChainSupportDetails;

void setupDebugCallback();
QueueFamilyIndices *findQueueFamilies(VkPhysicalDevice vkDevice, VkSurfaceKHR surface);
swapChainSupportDetails querySwapChainSupport(VkPhysicalDevice vkDevice, VkSurfaceKHR surface, unsigned int *formatCount, unsigned int *presentModeCount);
void pickPhysicalDevice(vkGraphics *g);
void createLogicalDevice(vkGraphics *g);
void createDescriptorSetLayout(vkGraphics *g);
void createCommandPool(vkGraphics *g);

void initGraphics(vkGraphics *g);

#endif