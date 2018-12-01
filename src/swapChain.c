#include "swapChain.h"

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, unsigned int formatCount) {
	if(availableFormats != NULL && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		VkSurfaceFormatKHR newFormat = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR};
		return newFormat;
	}

	for(unsigned int i = 0; i < formatCount; i++) {
		if(availableFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM && availableFormats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
			return availableFormats[i];
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR *availablePresentModes, unsigned int presentModeCount) {
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for(unsigned int i = 0; i < presentModeCount; i++) {
		if(availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentModes[i];
		}
		else if(availablePresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentModes[i];
		}
	}
	return bestMode;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR capabilities, GLFWwindow *window) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if(capabilities.currentExtent.width != (uintmax_t)(UINT32_MAX)) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = {width, height};

		actualExtent.width = max(capabilities.minImageExtent.width, min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = max(capabilities.minImageExtent.height, min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void createSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, vkSwapchain *s, GLFWwindow *window) {
	unsigned int formatCount;
	unsigned int presentModeCount;
	swapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface, &formatCount, &presentModeCount);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats, formatCount);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes, presentModeCount);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);
	unsigned int imageCount = swapChainSupport.capabilities.minImageCount + 1;
	s->deviceImageCount = imageCount;

	if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = surface,
		.minImageCount = imageCount,
		.imageFormat = surfaceFormat.format,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
	};

	QueueFamilyIndices *indices = findQueueFamilies(physicalDevice, surface);
	unsigned int queueFamilyIndices[] = {(unsigned int)indices->graphicsFamily, (unsigned int)indices->presentFamily};

	if (indices->graphicsFamily != indices->presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if(vkCreateSwapchainKHR(device, &createInfo, NULL, &s->swapChain) != VK_SUCCESS) {
		printf("Failed to create swap chain.\n");
		//cleanup();
	}

	vkGetSwapchainImagesKHR(device, s->swapChain, &imageCount, NULL);
	s->swapChainImages = malloc(imageCount*sizeof(VkImage));
	vkGetSwapchainImagesKHR(device, s->swapChain, &imageCount, s->swapChainImages);

	s->swapChainImageFormat = surfaceFormat.format;
	s->swapChainExtent = extent;
}

void createImageViews(VkDevice device, vkSwapchain *s) {
	s->swapChainImageViews = malloc(s->deviceImageCount*sizeof(VkImageView));
	for(unsigned int i = 0; i < s->deviceImageCount; i++) {
		VkImageViewCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = s->swapChainImages[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = s->swapChainImageFormat,
			.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.subresourceRange.baseMipLevel = 0,
			.subresourceRange.levelCount =1,
			.subresourceRange.baseArrayLayer = 0,
			.subresourceRange.layerCount = 1,
		};

		if(vkCreateImageView(device, &createInfo, NULL, &s->swapChainImageViews[i]) != VK_SUCCESS) {
			printf("Failed to create VK image views\n");
			//cleanup();
		}
	}
}

void createRenderPass(VkDevice device, vkSwapchain *s, VkFormat format) {
	VkAttachmentDescription colorAttachment = {
		.format = s->swapChainImageFormat,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};

	VkAttachmentDescription depthAttachment = {
		.format = format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};

	VkAttachmentReference colorAttachmentRef = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};

	VkAttachmentReference depthAttachmentRef = {
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};

	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentRef,
		.pDepthStencilAttachment = &depthAttachmentRef,
	};

	VkSubpassDependency dependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	};

	VkAttachmentDescription attachments[2];
	attachments[0] = colorAttachment;
	attachments[1] = depthAttachment;

	VkRenderPassCreateInfo renderPassInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency,
	};

	if(vkCreateRenderPass(device, &renderPassInfo, NULL, &s->renderPass) != VK_SUCCESS) {
		printf("Failed to create render pass.\n");
		//cleanup();
	}
}

void initSwapChainRenderPass(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, vkSwapchain *s, GLFWwindow *window, VkFormat format) {
	createSwapChain(device, physicalDevice, surface, s, window);
	createImageViews(device, s);
	createRenderPass(device, s, format);
}
