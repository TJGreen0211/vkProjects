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
	//unsigned int size;
} vertexData;

//typedef struct vertices {
//	unsigned int size;
//	unsigned int vertexNumber;
//	float *positions[3];
//	float *colors[3];
//	float *textureCoords[2];
//} vertices;

VkCommandBuffer beginSingleTimeCommands(VkDevice vkDevice, VkCommandPool pool);
void endSingleTimeCommands(VkDevice vkDevice, VkQueue queue, VkCommandBuffer commandBuffer, VkCommandPool pool);
void copyBuffer(VkDevice vkDevice, VkCommandPool pool, VkQueue queue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void createBuffer(VkPhysicalDevice physicalDev, VkDevice vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory);
void createVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, vertexData *verts, unsigned int vertSize, VkBuffer *vertexBuffer, VkDeviceMemory *vertexBufferMemory);
void createIndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, unsigned int vertSize, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer *indexBuffer, VkDeviceMemory *indexBufferMemory, uint16_t *verts);

#endif