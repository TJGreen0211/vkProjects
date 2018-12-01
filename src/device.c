#include "device.h"

const char *deviceExtension[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
const char *validationLayer[] = {"VK_LAYER_LUNARG_standard_validation"};

//VkResult CreateDebugReportCallbackExt(VkInstance instance, const VkDebugReportCallbackCreateInfoExt* pCreateInfo, ) {
//
//}

void setupDebugCallback() {
	//if(!enableValidationLayers) return;

	//VkDebugUtilsMessengerCreateInfoEXT createInfo = {
	//	.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
	//	.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
	//	.pfnCallback = debugCallback,
	//}
	//if (CreateDebugReportCallbackEXT(instance, &createInfo, NULL, &callback) != VK_SUCCESS) {
	//	printf("failed to set up debug callback");
	//	cleanup();
	//}
}

int checkComplete(int graphicsVal, int presentVal) {
	return graphicsVal >= 0 && presentVal >= 0;
}

QueueFamilyIndices *findQueueFamilies(VkPhysicalDevice vkDevice, VkSurfaceKHR surface) {
	QueueFamilyIndices *indices = malloc(sizeof(QueueFamilyIndices));
	indices->graphicsFamily = -1;
	indices->presentFamily = -1;
	indices->isComplete = checkComplete;

	unsigned int queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vkDevice, &queueFamilyCount, NULL);

	VkQueueFamilyProperties *queueFamilies = malloc(queueFamilyCount*sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(vkDevice, &queueFamilyCount, queueFamilies);
	for(int i = 0; i < (int)queueFamilyCount; i++) {
		if(queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices->graphicsFamily = i;
		}

		int presentSupport = 0;
		vkGetPhysicalDeviceSurfaceSupportKHR(vkDevice, i, surface, &presentSupport);

		if(queueFamilies[i].queueCount > 0 &&presentSupport) {
			indices->presentFamily = i;
		}
		if(indices->isComplete(indices->graphicsFamily, indices->presentFamily)) {
			break;
		}
	}
	return indices;
}

int checkDeviceExtensionSupported(VkPhysicalDevice vkDevice) {
	unsigned int extensionCount;
	vkEnumerateDeviceExtensionProperties(vkDevice, NULL, &extensionCount, NULL);

	VkExtensionProperties *availableExtensions = malloc(extensionCount*sizeof(VkExtensionProperties));
	vkEnumerateDeviceExtensionProperties(vkDevice, NULL, &extensionCount, availableExtensions);

	for(int i = 0; i < 1; i++) {
		unsigned int layerFound = 0;
		for(int j = 0; j < (int)extensionCount; j++) {
			if(strcmp(deviceExtension[i], availableExtensions[j].extensionName) == 0) {
				//printf("%s, %s\n", deviceExtensions[i], availableExtensions[j].extensionName);
				layerFound = 1;
				break;
			}
		}
		if(!layerFound) {
			return 0;
		}
	}
	free(availableExtensions);
	return 1;
}

swapChainSupportDetails querySwapChainSupport(VkPhysicalDevice vkDevice, VkSurfaceKHR surface, unsigned int *formatCount, unsigned int *presentModeCount) {
	swapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkDevice, surface, &details.capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(vkDevice, surface, formatCount, NULL);

	if(formatCount[0] != 0) {
		details.formats = malloc(formatCount[0]*sizeof(details.formats));
		vkGetPhysicalDeviceSurfaceFormatsKHR(vkDevice, surface, formatCount, details.formats);
	}
	else {
		details.formats = malloc(sizeof(details.formats));
		details.formats = NULL;
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(vkDevice, surface, presentModeCount, NULL);

	if(presentModeCount[0] != 0) {
		details.presentModes = malloc(formatCount[0]*sizeof(details.presentModes));
		vkGetPhysicalDeviceSurfacePresentModesKHR(vkDevice, surface, presentModeCount, details.presentModes);
	}
	else {
		details.presentModes = malloc(sizeof(details.presentModes));
		details.presentModes = NULL;
	}

	return details;
}

int isDeviceSuitable(VkPhysicalDevice vkDevice, VkSurfaceKHR surface) {
	//VkPhysicalDeviceProperties deviceProperties;
	//vkGetPhysicalDeviceProperties(vkDevice, &deviceProperties);
    //
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(vkDevice, &deviceFeatures);

	QueueFamilyIndices *indices = findQueueFamilies(vkDevice, surface);

	int extensionsSupported = checkDeviceExtensionSupported(vkDevice);

	int swapChainSuitable = 0;
	unsigned int formatCount;
	unsigned int presentModeCount;
	if(extensionsSupported) {
		swapChainSupportDetails swapChainSupport = querySwapChainSupport(vkDevice, surface, &formatCount, &presentModeCount);
		swapChainSuitable = swapChainSupport.formats != NULL && swapChainSupport.presentModes != NULL;
	}
	return indices->isComplete(indices->graphicsFamily, indices->presentFamily) && extensionsSupported && swapChainSuitable && deviceFeatures.samplerAnisotropy;
}

void pickPhysicalDevice(vkGraphics *g) {
	unsigned int deviceCount = 0;
	vkEnumeratePhysicalDevices(g->instance, &deviceCount, NULL);
	if(deviceCount == 0) {
		printf("GPU support for vulkan not available.\n");
		//cleanup();
	}
	VkPhysicalDevice *devices = malloc(deviceCount*sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(g->instance, &deviceCount, devices);
	for(int i = 0; i < (int)deviceCount; i++) {
		if(isDeviceSuitable(devices[i], g->surface)) {
			g->physicalDevice = devices[i];
			break;
		}
	}
	if(&g->physicalDevice == VK_NULL_HANDLE) {
		printf("Failed to find a suitable GPU.\n");
		//cleanup();
	}
	free(devices);
}

void createLogicalDevice(vkGraphics *g) {
	QueueFamilyIndices *indices = findQueueFamilies(g->physicalDevice, g->surface);
	float queuePriority = 1.0f;

	//TODO: Change for multiple queue families.

	//VkDeviceQueueCreateInfo *queueCreateInfo = malloc()

	VkDeviceQueueCreateInfo queueCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = indices->graphicsFamily,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority,
	};

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(g->physicalDevice, &deviceFeatures);

	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pQueueCreateInfos = &queueCreateInfo,
		.queueCreateInfoCount = 1,
		.pEnabledFeatures = &deviceFeatures,
		.enabledExtensionCount = sizeof(deviceExtension)/sizeof(deviceExtension[0]),
		.ppEnabledExtensionNames = deviceExtension,
		.enabledLayerCount = 0,
	};

	//TODO: fix this to use enableValidationLayers
	//if(enableValidationLayers) {
	if(1) {
		createInfo.enabledLayerCount = sizeof(validationLayer)/sizeof(validationLayer[0]);
		createInfo.ppEnabledLayerNames = validationLayer;
	}

	if(vkCreateDevice(g->physicalDevice, &createInfo, NULL, &g->device) != VK_SUCCESS) {
		printf("Failed to create logical device.\n");
		//cleanup();
	}

	vkGetDeviceQueue(g->device, indices->graphicsFamily, 0, &g->graphicsQueue);
	vkGetDeviceQueue(g->device, indices->presentFamily, 0, &g->presentQueue);
}

void createDescriptorSetLayout(vkGraphics *g) {
	VkDescriptorSetLayoutBinding uboLayoutBinding = {
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.pImmutableSamplers = NULL,
	};

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {
		.binding = 1,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.pImmutableSamplers = NULL,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
	};

	VkDescriptorSetLayoutBinding *bindings = malloc(2*sizeof(VkDescriptorSetLayoutBinding));
	bindings[0] = uboLayoutBinding;
	bindings[1] = samplerLayoutBinding;

	VkDescriptorSetLayoutCreateInfo layoutInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = 2,
		.pBindings = bindings,
	};

	if (vkCreateDescriptorSetLayout(g->device, &layoutInfo, NULL, &g->descriptorSetLayout) != VK_SUCCESS) {
		printf("Failed to create descriptor set layout.");
		//cleanup();
	}
}

void createCommandPool(vkGraphics *g) {
	QueueFamilyIndices *queueFamilyIndices = findQueueFamilies(g->physicalDevice, g->surface);

	VkCommandPoolCreateInfo poolInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = queueFamilyIndices->graphicsFamily,
		.flags = 0,
	};

	if(vkCreateCommandPool(g->device, &poolInfo, NULL, &g->commandPool) != VK_SUCCESS) {
		printf("Failed to create command pool.\n");
	}
}

void initGraphics(vkGraphics *g) {
	g->physicalDevice = VK_NULL_HANDLE;
	pickPhysicalDevice(g);
	createLogicalDevice(g);
	createDescriptorSetLayout(g);
	createCommandPool(g);
}