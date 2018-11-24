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