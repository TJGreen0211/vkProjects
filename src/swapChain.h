#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#define GLFW_INCLUDE_VULKAN

#include <windows.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "device.h"

typedef struct vkSwapchain {
	VkSwapchainKHR swapChain;
	VkExtent2D swapChainExtent;
	VkImage *swapChainImages;
	VkFormat swapChainImageFormat;
	VkImageView *swapChainImageViews;
	unsigned int deviceImageCount;
} vkSwapchain;

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, unsigned int formatCount);
VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR *availablePresentModes, unsigned int presentModeCount);
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR capabilities, GLFWwindow *window);
void createSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, vkSwapchain *s, GLFWwindow *window);

#endif