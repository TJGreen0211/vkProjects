#define GLFW_INCLUDE_VULKAN

#include <windows.h>
#include <GLFW/glfw3.h>
#include <tinyobj/tinyobj_loader_c.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "linearAlg.h"
#include "arcballCamera.h"
#include "window.h"
#include "buffer.h"
#include "instance.h"
#include "device.h"
#include "swapChain.h"
#include "textures.h"
#include "shader.h"
#include "sphere.h"
#include "quadCube.h"

#define _CRT_SECURE_NO_DEPRECATE 1
//#define _CRT_SECURE_NO_WARNINGS 1

//#define max(x,y) ((x) >= (y)) ? (x) : (y)
//#define min(x,y) ((x) <= (y)) ? (x) : (y)

//#include "vulkan_core.h"

#ifndef NDEBUG
	const unsigned int enableValidationLayers = 0;
#else
	const unsigned int enableValidationLayers = 1;
#endif

double deltaTime = 0.0;
double lastFrame = 0.0;

struct texture_object {
    VkSampler sampler;

    VkImage image;
    VkImageLayout imageLayout;

    VkMemoryAllocateInfo mem_alloc;
    VkDeviceMemory mem;
    VkImageView view;
    int32_t tex_width, tex_height;
};

//typedef struct SwapchainImageResources {
//    VkImage image;
//    VkCommandBuffer cmd;
//    VkCommandBuffer graphics_to_present_cmd;
//    VkImageView view;
//    VkBuffer uniform_buffer;
//    VkDeviceMemory uniform_memory;
//    VkFramebuffer framebuffer;
//    VkDescriptorSet descriptor_set;
//} SwapchainImageResources;

//typedef struct pipeline {
//
//} pipeline;


vkGraphics graphics;
vkSwapchain graphicsSwapchain;
vkBuffer graphicsBuffer;
vkTexture depthTexture;
vkTexture imageTexture;

const char *deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

vertexData *vertex;

sphere newSphere;
quadCube cube;

uint32_t vertexIndices[12] = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};

VkPipelineLayout pipelineLayout;
VkPipeline graphicsPipeline;
VkCommandBuffer *commandBuffers;
GLFWwindow *window;

VkDescriptorPool descriptorPool;
VkDescriptorSet *descriptorSets;

VkSemaphore imageAvailableSemaphore;
VkSemaphore renderFinishedSemaphore;

VkDebugReportCallbackEXT callback;

static VkVertexInputBindingDescription getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription ={
		.binding = 0,
		.stride = sizeof(vertexData),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
	};
	return bindingDescription;
}

static VkVertexInputAttributeDescription *getAttributeDescriptions() {
	VkVertexInputAttributeDescription *attributeDescriptions = malloc(3*sizeof(VkVertexInputAttributeDescription));
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(vertexData, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(vertexData, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(vertexData, texCoord);

	return attributeDescriptions;
}

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback
        , const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugReportCallbackEXT func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    //if (func != NULL) {
    //    func(instance, callback, pAllocator);
    //}
}

void cleanupSwapChain() {
	for(unsigned int i = 0; i < graphicsSwapchain.deviceImageCount; i++) {
		vkDestroyFramebuffer(graphics.device, graphicsSwapchain.swapChainFramebuffers[i], NULL);
	}
	vkFreeCommandBuffers(graphics.device, graphics.commandPool, graphicsSwapchain.deviceImageCount, commandBuffers);

	vkDestroyPipeline(graphics.device, graphicsPipeline, NULL);
	vkDestroyPipelineLayout(graphics.device, pipelineLayout, NULL);
	vkDestroyRenderPass(graphics.device, graphicsSwapchain.renderPass, NULL);

	for(unsigned int i = 0; i < graphicsSwapchain.deviceImageCount; i++) {
		vkDestroyImageView(graphics.device, graphicsSwapchain.swapChainImageViews[i], NULL);
	}

	vkDestroySwapchainKHR(graphics.device, graphicsSwapchain.swapChain, NULL);
}

void cleanup() {
	cleanupSwapChain();
	vkDestroyImageView(graphics.device, depthTexture.imageView, NULL);
    vkDestroyImage(graphics.device, depthTexture.image, NULL);
    vkFreeMemory(graphics.device, depthTexture.imageMemory, NULL);

	vkDestroySampler(graphics.device, graphics.textureSampler, NULL);
	vkDestroyImageView(graphics.device, graphics.textureImageView, NULL);
	vkDestroyImage(graphics.device, imageTexture.image, NULL);
	vkFreeMemory(graphics.device, imageTexture.imageMemory, NULL);

	vkDestroyDescriptorPool(graphics.device, descriptorPool, NULL);
	vkDestroyDescriptorSetLayout(graphics.device, graphics.descriptorSetLayout, NULL);

	for (unsigned int i = 0; i < graphicsSwapchain.deviceImageCount; i++) {
		vkDestroyBuffer(graphics.device, graphicsBuffer.uniformBuffer[i], NULL);
		vkFreeMemory(graphics.device, graphicsBuffer.uniformBufferMemory[i], NULL);
	}

	vkDestroyBuffer(graphics.device, graphicsBuffer.indexBuffer, NULL);
	vkFreeMemory(graphics.device, graphicsBuffer.indexBufferMemory, NULL);

	vkDestroyBuffer(graphics.device, graphicsBuffer.vertexBuffer, NULL);
	vkFreeMemory(graphics.device, graphicsBuffer.vertexBufferMemory, NULL);

	vkDestroySemaphore(graphics.device, renderFinishedSemaphore, NULL);
	vkDestroySemaphore(graphics.device, imageAvailableSemaphore, NULL);
	vkDestroyCommandPool(graphics.device, graphics.commandPool, NULL);

	vkDestroyDevice(graphics.device, NULL);
	DestroyDebugReportCallbackEXT(graphics.instance, callback, NULL);
	vkDestroySurfaceKHR(graphics.instance, graphics.surface, NULL);
	vkDestroyInstance(graphics.instance, NULL);

	glfwDestroyWindow(window);
	glfwTerminate();
	printf("Cleanup done\n");
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {
		printf("Validation layer: %s\n", msg);
	return VK_FALSE;
}


VkShaderModule createShaderModule(const char* code, size_t shaderSize) {
	//uint32_t *pCodeArray = malloc(shaderSize*sizeof(uint32_t));
	//for(int i = 0; i < shaderSize; i++) {
	//	pCodeArray[i] = (uint32_t)code[i];
	//}

	VkShaderModuleCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = shaderSize,
		.pCode = (uint32_t*)code,
	};

	VkShaderModule shaderModule;
	if(vkCreateShaderModule(graphics.device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
		printf("Failed to create shader module.\n");
		cleanup();
	}
	return shaderModule;
}

VkFormat findSupportedFormat(VkFormat *candidates, int numFormats, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for(int i = 0; i < numFormats; i++) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(graphics.physicalDevice, candidates[i], &props);

		if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return candidates[i];
		} else if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return candidates[i];
		}
	}

	printf("No device formats supported.");
	cleanup();
	return 0;
}

VkFormat findDepthFormat() {
	VkFormat candidates[3] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
	return findSupportedFormat(candidates, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void createGraphicsPipeline() {

	size_t vertSize, fragSize;
	char *vertShaderCode = readShader("shaders/vert.spv", &vertSize);
	char *fragShaderCode = readShader("shaders/frag.spv", &fragSize);

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, vertSize);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, fragSize);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vertShaderModule,
		.pName = "main",
	};

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = fragShaderModule,
		.pName = "main",
	};

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};


	VkVertexInputBindingDescription bindingDescription = getBindingDescription();
	VkVertexInputAttributeDescription *attributeDescriptions;
	attributeDescriptions = getAttributeDescriptions();
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &bindingDescription,
		.vertexAttributeDescriptionCount = 3,
		.pVertexAttributeDescriptions = attributeDescriptions,
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE,
	};

	VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)graphicsSwapchain.swapChainExtent.width,
		.height = (float)graphicsSwapchain.swapChainExtent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	VkRect2D scissor = {
		.offset = {0, 0},
		.extent = graphicsSwapchain.swapChainExtent,
	};

	VkPipelineViewportStateCreateInfo viewportState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor,
	};

	VkPipelineRasterizationStateCreateInfo rasterizer = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.lineWidth = 1.0f,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f,
	};

	VkPipelineMultisampleStateCreateInfo multisampling = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.sampleShadingEnable = VK_FALSE,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.minSampleShading = 1.0f,
		.pSampleMask = NULL,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE,
	};

	VkPipelineDepthStencilStateCreateInfo depthStencil = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
		.depthBoundsTestEnable = VK_FALSE,
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 1.0f,
		.stencilTestEnable = VK_FALSE,
	};

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		.blendEnable = VK_FALSE,
		//.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		//.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		//.colorBlendOp = VK_BLEND_OP_ADD,
		//.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		//.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		//.alphaBlendOP = VK_BLEND_OP_ADD,
	};

	//colorBlendAttachment.blendescriptorSetLayoutdEnable = VK_TRUE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &colorBlendAttachment,
		.blendConstants[0] = 0.0f,
		.blendConstants[1] = 0.0f,
		.blendConstants[2] = 0.0f,
		.blendConstants[3] = 0.0f
	};

	//VkDynamicState dynamicStates[] = {
	//	VK_DYNAMIC_STATE_VIEWPORT,
	//	VK_DYNAMIC_STATE_LINE_WIDTH,
	//};
	//VkPipelineDynamicStateCreate_info dynamicState = {
	//	.sType = VVK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
	//	.dynamicStateCount = 2,
	//	.dynamicStates = dynamicStates
	//};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 1,
		.pSetLayouts = &graphics.descriptorSetLayout,
		//.pSetLayouts = NULL,
		//.pushConstantRangeCount = 0,
		//.pPushConstantRanges = 0,
	};

	if(vkCreatePipelineLayout(graphics.device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
		printf("Failed to create pipeline layout.\n");
		cleanup();
	};

	VkGraphicsPipelineCreateInfo pipelineInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = 2,
		.pStages = shaderStages,
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,
		.pDepthStencilState = &depthStencil,
		.pColorBlendState = &colorBlending,
		.pDynamicState = NULL,
		.layout = pipelineLayout,
		.renderPass = graphicsSwapchain.renderPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1,
	};

	if(vkCreateGraphicsPipelines(graphics.device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline) != VK_SUCCESS) {
		printf("Failed to create graphics pipeline.\n");
		cleanup();
	}

	vkDestroyShaderModule(graphics.device, vertShaderModule, NULL);
	vkDestroyShaderModule(graphics.device, fragShaderModule, NULL);

	//printf("%zd, %zd\n", vertSize, fragSize);
}

void createFramebuffers(VkDevice device, vkSwapchain *s, VkImageView depthImageView) {
	s->swapChainFramebuffers = malloc(s->deviceImageCount*sizeof(VkFramebuffer));
	for(unsigned int i = 0; i < s->deviceImageCount; i++) {
		VkImageView attachments[2] = {
			s->swapChainImageViews[i],
			depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = s->renderPass,
			.attachmentCount = 2,
			.pAttachments = attachments,
			.width = s->swapChainExtent.width,
			.height = s->swapChainExtent.height,
			.layers = 1,
		};

		if(vkCreateFramebuffer(device, &framebufferInfo, NULL, &s->swapChainFramebuffers[i]) != VK_SUCCESS) {
			printf("Failed to create framebuffer.\n");
			cleanup();
		}
	}
}

void createCommandBuffer() {
	commandBuffers = malloc(graphicsSwapchain.deviceImageCount*sizeof(VkCommandBuffer));
	VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = graphics.commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = (uint32_t)graphicsSwapchain.deviceImageCount,
	};

	if(vkAllocateCommandBuffers(graphics.device, &allocInfo, commandBuffers) != VK_SUCCESS) {
		printf("Failed to allocate command buffers.\n");
		cleanup();
	}

	for(unsigned int i = 0; i < graphicsSwapchain.deviceImageCount; i++) {
		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			.pInheritanceInfo = NULL,
		};
		vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

		VkRenderPassBeginInfo renderPassInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = graphicsSwapchain.renderPass,
			.framebuffer = graphicsSwapchain.swapChainFramebuffers[i],
			.renderArea.offset = {0, 0},
			.renderArea.extent = graphicsSwapchain.swapChainExtent,
		};

		VkClearValue clearColor = {.color = {0.0f, 1.0f, 0.0f, 1.0f},};
		VkClearValue clearDepth = {.depthStencil = {1.0f, 0},};

		VkClearValue clearValues[2];// = malloc(2*sizeof(VkClearValue));
		clearValues[0] = clearColor;
		clearValues[1] = clearDepth;

		//VkClearValue clearColor = {0.0f, 0.1f, 0.3f, 1.0f};
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
			VkBuffer vertexBuffers[] = {graphicsBuffer.vertexBuffer};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
			//vkCmdBindIndexBuffer(commandBuffers[i], graphicsBuffer.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, NULL);
			vkCmdDraw(commandBuffers[i], (uint32_t)(cube.vertexNumber*sizeof(vertexData)/sizeof(vertex[0])), 1, 0, 0);
			//vkCmdDrawIndexed(commandBuffers[i], (uint32_t)(sizeof(vertexIndices)/sizeof(vertexIndices[0])), 1, 0, 0, 0);
			//printf("(uint32_t)sizeof(vertices): %d\n", (uint32_t)sizeof(vertices)/sizeof(vertices[0]));

		vkCmdEndRenderPass(commandBuffers[i]);

		if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			printf("Failed to record command buffer.\n");
			cleanup();
		}
	}
}

void createSemaphores() {
	VkSemaphoreCreateInfo semaphoreInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	};
	if(vkCreateSemaphore(graphics.device, &semaphoreInfo, NULL, &imageAvailableSemaphore) != VK_SUCCESS
	|| vkCreateSemaphore(graphics.device, &semaphoreInfo, NULL, &renderFinishedSemaphore) != VK_SUCCESS) {
		printf("Failed to create semaphores.\n");
		cleanup();
	}
}

void createDescriptorSets() {
	VkDescriptorSetLayout *layouts;
	layouts = malloc(sizeof(VkDescriptorSetLayout)*graphicsSwapchain.deviceImageCount);

	for(unsigned int i = 0; i < graphicsSwapchain.deviceImageCount; i++) {
		layouts[i] = graphics.descriptorSetLayout;
	}
	VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = descriptorPool,
		.descriptorSetCount = graphicsSwapchain.deviceImageCount,
		.pSetLayouts = layouts,
	};

	descriptorSets = malloc(sizeof(VkDescriptorSet)*graphicsSwapchain.deviceImageCount);
	if(vkAllocateDescriptorSets(graphics.device, &allocInfo, descriptorSets) != VK_SUCCESS) {
		printf("Failed to allocate descriptor sets.");
	}

	for(unsigned int i = 0; i < graphicsSwapchain.deviceImageCount; i++) {
		VkDescriptorBufferInfo bufferInfo = {
			.buffer = graphicsBuffer.uniformBuffer[i],
			.offset = 0,
			.range = sizeof(uniformBufferObject),
		};

		VkDescriptorImageInfo imageInfo = {
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			.imageView = graphics.textureImageView,
			.sampler = graphics.textureSampler,
		};

		VkWriteDescriptorSet descriptorWrite = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets[i],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.pBufferInfo = &bufferInfo,
			.pImageInfo = NULL,
			.pTexelBufferView = NULL,
		};

		VkWriteDescriptorSet descriptorWriteImage = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets[i],
			.dstBinding = 1,
			.dstArrayElement = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.pImageInfo = &imageInfo,
		};

		VkWriteDescriptorSet *descriptorWrites = malloc(2*sizeof(VkWriteDescriptorSet));
		descriptorWrites[0] = descriptorWrite;
		descriptorWrites[1] = descriptorWriteImage;

		vkUpdateDescriptorSets(graphics.device, 2, descriptorWrites, 0, NULL);
	}
}

void createDescriptorPool() {

	VkDescriptorPoolSize *poolSizes = malloc(2*sizeof(VkDescriptorPoolSize));
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = graphicsSwapchain.deviceImageCount;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = graphicsSwapchain.deviceImageCount;

	VkDescriptorPoolCreateInfo poolInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.poolSizeCount = 2,
		.pPoolSizes = poolSizes,
		.maxSets = graphicsSwapchain.deviceImageCount,
	};

	if(vkCreateDescriptorPool(graphics.device, &poolInfo, NULL, &descriptorPool) != VK_SUCCESS) {
		printf("Failed to create descriptor pool.");
		cleanup();
	}
}

void createTextureImageView(VkDevice device, VkImage textureImage) {
	graphics.textureImageView = createImageView(device, textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

void createDepthResources(vkTexture *t) {
	t->format = findDepthFormat();

	createImage(graphics.device, graphics.physicalDevice, graphicsSwapchain.swapChainExtent.width, graphicsSwapchain.swapChainExtent.height, t->format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &t->image, &t->imageMemory);
	t->imageView = createImageView(graphics.device, t->image, t->format, VK_IMAGE_ASPECT_DEPTH_BIT);


	transitionImageLayout(graphics.device, graphics.commandPool, graphics.graphicsQueue, t->image, t->format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	}

void loadModel() {
	//tinyobj_attrib_t attrib;
	//tinyobj_shape_t *shapes;
	//tinyobj_material_t *material;
	//size_t numMaterials;
	//size_t numShapes;
	//size_t dataLen = 0;

	//const char *data = get_file_data(&dataLen, "");
	//if(data == NULL) {
	//	printf("Failed to load object file.");
	//	exit("-1")
	//}
}

void initVulkan() {
	createInstance(enableValidationLayers, &graphics.instance);
	setupDebugCallback();
	createSurface(graphics.instance, &graphics.surface, window);

	initGraphics(&graphics);
	initSwapChainRenderPass(graphics.device, graphics.physicalDevice, graphics.surface, &graphicsSwapchain, window, findDepthFormat());

	createGraphicsPipeline();

	createDepthResources(&depthTexture);
	createFramebuffers(graphics.device, &graphicsSwapchain, depthTexture.imageView);

	createTextureImage(graphics.physicalDevice, graphics.device, graphics.commandPool, graphics.graphicsQueue, &imageTexture);
	createTextureImageView(graphics.device, imageTexture.image);
	createTextureSampler(graphics.device, &graphics.textureSampler);


	newSphere = tetrahedron(4, &newSphere);
	//int size;
	//int nsize;
	//vec3 *points;
	//vec3 *normals;
	//int vertexNumber;

	//{{-0.5f, -0.5f, 0.0f}, 	{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    //{{0.5f, -0.5f, 0.0f}, 	{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    //{{0.5f, 0.5f, 0.0f}, 	{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	//{{0.5f, 0.5f, 0.0f}, 	{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    //{{-0.5f, 0.5f, 0.0f}, 	{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
	//{{-0.5f, -0.5f, 0.0f}, 	{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

	float positions[6][3] = {
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{0.5f, 0.5f, 0.0f},
		{0.5f, 0.5f, 0.0f},
		{-0.5f, 0.5f, 0.0f},
		{-0.5f, -0.5f, 0.0f},
	};
	float normals[6][3] = {
		{0.0f, 1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 0.0f},
	};
	float texCoords[6][3] = {
		{1.0f, 0.0f},
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
	};

	createCube(5, &cube);


	vertex = malloc(cube.vertexNumber*sizeof(vertexData));
	for(int i = 0; i < cube.vertexNumber; i++){
		vertex[i].pos[0] = (float)cube.points[i].x;
		vertex[i].pos[1] = (float)cube.points[i].y;
		vertex[i].pos[2] = (float)cube.points[i].z;

		vertex[i].color[0] = (float)cube.normals[i].x;
		vertex[i].color[1] = (float)cube.normals[i].y;
		vertex[i].color[2] = (float)cube.normals[i].z;

		vertex[i].texCoord[0] = (float)cube.normals[i].x;
		vertex[i].texCoord[1] = (float)cube.normals[i].y;


		//memcpy(&vertex[i].pos, &positions[i], sizeof(positions[0]));
		//memcpy(&vertex[i].color, &normals[i], sizeof(normals[0]));
		//memcpy(&vertex[i].texCoord, &texCoords[i], sizeof(texCoords[0]));

		//printf("position[%d]: %f, %f, %f\n", i, vertex[i].pos[0], vertex[i].pos[1], vertex[i].pos[2]);
		//printf("color[%d]: %f, %f, %f\n", i, vertex[i].color[0], vertex[i].color[1], vertex[i].color[2]);
		//printf("tex[%d]: %f, %f\n", i, vertex[i].texCoord[0], vertex[i].texCoord[1]);
	}


	loadModel();
	createVertexBuffer(graphics.device, graphics.physicalDevice, graphics.commandPool, graphics.graphicsQueue, vertex, cube.vertexNumber*sizeof(vertexData), &graphicsBuffer);
	createIndexBuffer(graphics.device, graphics.physicalDevice, sizeof(vertexIndices), graphics.commandPool, graphics.graphicsQueue, &graphicsBuffer, vertexIndices);
	createUniformBuffer(graphics.device, graphics.physicalDevice, graphicsSwapchain.deviceImageCount, &graphicsBuffer);

	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffer();
	createSemaphores();
}

void recreateSwapChain() {
	int width, height;
    glfwGetWindowSize(window, &width, &height);
    if (width == 0 || height == 0) return;
	vkDeviceWaitIdle(graphics.device);

	cleanupSwapChain();
	initSwapChainRenderPass(graphics.device, graphics.physicalDevice, graphics.surface, &graphicsSwapchain, window, findDepthFormat());

	createGraphicsPipeline();

	createDepthResources(&depthTexture);
	createFramebuffers(graphics.device, &graphicsSwapchain, depthTexture.imageView);
	createCommandBuffer();
}


static void onWindowResized(GLFWwindow *window, int width, int height) {
	recreateSwapChain();
}

void drawFrame() {

	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(graphics.device, graphicsSwapchain.swapChain, UINT_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	if(result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		printf("Failed to acquire swap chain image.\n");
		cleanup();
	}
	mat4 m = translate(0.0, -1.0, 0.0);
	vec3 arcBallPos = getCamera();
	m = multiplymat4(multiplymat4(translate(-arcBallPos.x, -arcBallPos.y, -arcBallPos.z), rotateX(0.0)), scale(0.5));

	mat4 v = getViewMatrix();
	mat4 p = perspective(45.0, graphicsSwapchain.swapChainExtent.width / graphicsSwapchain.swapChainExtent.height, 0.1, 100000);
	//p.m[1][1] *= -1;
	updateUniformBuffer(graphics.device, graphicsBuffer.uniformBufferMemory[imageIndex], m, v, p);
	doMovement(deltaTime);

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
	};
	VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if(vkQueueSubmit(graphics.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		printf("Failed to submit draw command buffer.\n");
		cleanup();
	}

	VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphores,
	};

	VkSwapchainKHR swapChains[] = {graphicsSwapchain.swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(graphics.presentQueue, &presentInfo);

	if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreateSwapChain();
	}
	else if(result != VK_SUCCESS) {
		printf("Failed to present swap chain image.\n");
		cleanup();
	}

	vkQueueWaitIdle(graphics.presentQueue);
}

void mainLoop() {
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		drawFrame();
	}
	vkDeviceWaitIdle(graphics.device);
}

void run() {
	window = initWindow();
	glfwSetWindowSizeCallback(window, onWindowResized);
	if (window == NULL)
	{
		printf("Failed to create GLFW window.");
		glfwTerminate();
		cleanup();
	}
	if(!glfwVulkanSupported()) {
		printf("Vulkan not supported.");
		cleanup();
	}
	initVulkan();
	mainLoop();

	cleanup();
}


int main(int argc, char *argv[]) {
	run();
	printf("Program finished");
	return 0;
}