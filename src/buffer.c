#include "buffer.h"

VkCommandBuffer beginSingleTimeCommands(VkDevice vkDevice, VkCommandPool pool) {
	VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandPool = pool,
		.commandBufferCount = 1,
	};
	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(vkDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void endSingleTimeCommands(VkDevice vkDevice, VkQueue queue, VkCommandBuffer commandBuffer, VkCommandPool pool) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer,
	};
	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(vkDevice, pool, 1, &commandBuffer);
}


void copyBuffer(VkDevice vkDevice, VkCommandPool pool, VkQueue queue, VkBuffer srcBuffer,
	VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(vkDevice, pool);

	VkBufferCopy copyRegion = {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = size,
	};
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(vkDevice, queue, commandBuffer, pool);
}

uint32_t findBufferMemoryType(VkPhysicalDevice physicalDev, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDev, &memProperties);

	for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if(typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	printf("Failed to find a suitable memory type.\n");
	//cleanup();
	return 2;
}

void createBuffer(VkPhysicalDevice physicalDev, VkDevice vkDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory) {
		VkBufferCreateInfo bufferInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};

	if(vkCreateBuffer(vkDevice, &bufferInfo, NULL, buffer) != VK_SUCCESS) {
		printf("Failed to create vertex buffer.\n");
		//cleanup();
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vkDevice, *buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findBufferMemoryType(physicalDev, memRequirements.memoryTypeBits, properties),
	};

	if(vkAllocateMemory(vkDevice, &allocInfo, NULL, bufferMemory) != VK_SUCCESS) {
		printf("Failed to allocate vertex buffer memory.\n");
		//cleanup();
	}
	vkBindBufferMemory(vkDevice, *buffer, *bufferMemory, 0);
}

void createVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, vertexData *verts, unsigned int vertSize, vkBuffer *b) {
	VkDeviceSize size = vertSize;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(physicalDevice, device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

	void *data;
	vkMapMemory(device, stagingBufferMemory, 0, size, 0, &data);
	memcpy(data, verts, vertSize);
	//printf("asdf: %zu\nS", sizeof(vertices));
	//for(int i = 0; i < 15; i++) {
	//	printf("%f\n", *((float *) ((char *) data + sizeof(float) * i)));
	//	//printf("%f %f\n", testData->color.x, testData->color.y);
	//}
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(physicalDevice, device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &b->vertexBuffer, &b->vertexBufferMemory);
	copyBuffer(device, commandPool, graphicsQueue, stagingBuffer, b->vertexBuffer, size);

	vkDestroyBuffer(device, stagingBuffer, NULL);
	vkFreeMemory(device, stagingBufferMemory, NULL);
}

void createIndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, unsigned int vertSize, VkCommandPool commandPool, VkQueue graphicsQueue, vkBuffer *b, uint16_t *verts) {
	VkDeviceSize size = vertSize;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(physicalDevice, device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

	void *data;
	vkMapMemory(device, stagingBufferMemory, 0, size, 0, &data);
	memcpy(data, verts, vertSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(physicalDevice, device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &b->indexBuffer, &b->indexBufferMemory);
	copyBuffer(device, commandPool, graphicsQueue, stagingBuffer, b->indexBuffer, size);

	vkDestroyBuffer(device, stagingBuffer, NULL);
	vkFreeMemory(device, stagingBufferMemory, NULL);
}

void updateUniformBuffer(VkDevice device, VkDeviceMemory bufferMemory, mat4 m, mat4 v, mat4 p) {

	uniformBufferObject ubo[1] = {
		{{
		(float)m.m[0][0], (float)m.m[0][1], (float)m.m[0][2], (float)m.m[0][3],
		(float)m.m[1][0], (float)m.m[1][1], (float)m.m[1][2], (float)m.m[1][3],
		(float)m.m[2][0], (float)m.m[2][1], (float)m.m[2][2], (float)m.m[2][3],
		(float)m.m[3][0], (float)m.m[3][1], (float)m.m[3][2], (float)m.m[3][3]},
		{
		(float)v.m[0][0], (float)v.m[0][1], (float)v.m[0][2], (float)v.m[0][3],
		(float)v.m[1][0], (float)v.m[1][1], (float)v.m[1][2], (float)v.m[1][3],
		(float)v.m[2][0], (float)v.m[2][1], (float)v.m[2][2], (float)v.m[2][3],
		(float)v.m[3][0], (float)v.m[3][1], (float)v.m[3][2], (float)v.m[3][3]},
		{
		(float)p.m[0][0], (float)p.m[0][1], (float)p.m[0][2], (float)p.m[0][3],
		(float)p.m[1][0], (float)p.m[1][1], (float)p.m[1][2], (float)p.m[1][3],
		(float)p.m[2][0], (float)p.m[2][1], (float)p.m[2][2], (float)p.m[2][3],
		(float)p.m[3][0], (float)p.m[3][1], (float)p.m[3][2], (float)p.m[3][3]}}

	};

	void *data;
	vkMapMemory(device, bufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, (const void *)&ubo[0], sizeof(ubo));
	vkUnmapMemory(device, bufferMemory);

	//for(int i = 0; i < 48; i++) {
	//	printf("%f, ", *((float *) ((char *) data + sizeof(float) * i)));
	//	if((i+1)%4 == 0) printf("\n");
	//}
	//printf("\n");
}