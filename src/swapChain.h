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
	VkRenderPass renderPass;
	VkFramebuffer *swapChainFramebuffers;
} vkSwapchain;

void initSwapChainRenderPass(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, vkSwapchain *s, GLFWwindow *window, VkFormat format);

#endif