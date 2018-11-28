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