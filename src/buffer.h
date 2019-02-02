#ifndef BUFFER_H
#define BUFFER_H

#define GLFW_INCLUDE_VULKAN

#include <windows.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <linearAlg.h>
#include "arcballCamera.h"
#include "device.h"


typedef struct uniformBufferObject {
	float model[16];
	float view[16];
	float projection[16];
	float camera[3];
} uniformBufferObject;

typedef struct vkBuffer {
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkBuffer *uniformBuffer;
	VkDeviceMemory *uniformBufferMemory;

} vkBuffer;

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
void createBuffer(VkPhysicalDevice physicalDev, VkDevice vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory);
void createVertexBuffer(vkGraphics g, vertexData *verts, unsigned int vertSize, vkBuffer *b);
void createIndexBuffer(vkGraphics g, unsigned int vertSize, vkBuffer *b, uint32_t *verts);
void updateUniformBuffer(VkDevice device, VkDeviceMemory bufferMemory, mat4 m, mat4 v, mat4 p);
void createUniformBuffer(vkGraphics, unsigned int imageCount, vkBuffer *b);

#endif