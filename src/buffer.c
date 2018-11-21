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


//void createVertexBuffer(VkPhysicalDevice physicalDev, VkDevice vkDevice, vertexData vertices, VkDeviceSize size, VkBuffer *buffer, VkDeviceMemory *bufferMemory) {
//	VkBuffer stagingBuffer;
//	VkDeviceMemory stagingBufferMemory;
//	createBuffer(physicalDev, vkDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
//	//createBuffer(vkDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
//
//	void *data;
//	vkMapMemory(vkDevice, stagingBufferMemory, 0, size, 0, &data);
//	memcpy(data, vertices, size);
//	vkUnmapMemory(vkDevice, stagingBufferMemory);
//
//	createBuffer(physicalDev, vkDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &buffer, &bufferMemory);
//	//createBuffer(vkDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &buffer, &bufferMemory);
//	copyBuffer(vkDevice, commandPool, graphicsQueue, stagingBuffer, vertexBuffer, size);
//
//	vkDestroyBuffer(vkDevice, stagingBuffer, NULL);
//	vkFreeMemory(vkDevice, stagingBufferMemory, NULL);
//}
