#ifndef BUFFER_H
#define BUFFER_H

#define GLFW_INCLUDE_VULKAN

#include <windows.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

typedef struct vertexData {
	float pos[3];
	float color[3];
	float texCoord[2];
	//VkVertexInputBindingDescription (*getBindingDescription);
	//VkVertexInputAttributeDescription (*getAttributeDescriptions);
} vertexData;

VkCommandBuffer beginSingleTimeCommands(VkDevice vkDevice, VkCommandPool pool);
void endSingleTimeCommands(VkDevice vkDevice, VkQueue queue, VkCommandBuffer commandBuffer, VkCommandPool pool);
void copyBuffer(VkDevice vkDevice, VkCommandPool pool, VkQueue queue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void createBuffer(VkPhysicalDevice physicalDev, VkDevice vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory);

void asdf(VkDevice *vkDevice, vertexData vertices, VkDeviceSize size, VkBuffer *buffer, VkDeviceMemory *bufferMemory);

#endif