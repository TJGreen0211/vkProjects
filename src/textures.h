#ifndef TEXTURES_H
#define TEXTURES_H

#define GLFW_INCLUDE_VULKAN

#include <windows.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <STB/stb_image.h>
#include "buffer.h"

typedef struct vkTexture {
	VkImage image;
    VkImageLayout imageLayout;
	VkDeviceMemory imageMemory;
	unsigned int width;
	unsigned int height;
} vkTexture;

void createImage(VkDevice device, VkPhysicalDevice physicalDevice, unsigned int width, unsigned int height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *imageMemory);
void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
void createTextureImage(VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage *textureImage, VkDeviceMemory *textureImageMemory);
VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
void createTextureSampler(VkDevice device, VkSampler *textureSampler);

#endif