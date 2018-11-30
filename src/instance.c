#include "instance.h"

const char *validationLayers[] = {"VK_LAYER_LUNARG_standard_validation"};

unsigned int checkValidationLayerSupport(const char **validation) {
	unsigned int layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, NULL);

	VkLayerProperties *availableLayers = malloc(layerCount*sizeof(VkLayerProperties));
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

	for(int i = 0; i < sizeof(validation)/sizeof(validation[0]); i++) {
		unsigned int layerFound = 0;
		for(int j = 0; j < (int)layerCount; j++) {
			if(strcmp(validation[i], availableLayers[j].layerName) == 0) {
				printf("%s, %s\n", validation[i], availableLayers[j].layerName);
				layerFound = 1;
				break;
			}
		}
		if(!layerFound) {
			return 0;
		}
	}
	free(availableLayers);
	return 1;
}

const char **getRequiredExtensions(unsigned int enableValidation, unsigned int *extensionCount) {
	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	*extensionCount = glfwExtensionCount;

	if(enableValidation) {
		*extensionCount = *extensionCount + 1;
	}
	const char **extensions = malloc(*extensionCount*sizeof(glfwExtensions[0]));

	for(int i = 0; i < (int)*extensionCount; i++) {
		extensions[i] = i < (int)glfwExtensionCount ? glfwExtensions[i] : VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
	}

	return extensions;
}

void createInstance(unsigned int enableValidation, VkInstance *instance) {
	if(enableValidation && !checkValidationLayerSupport(validationLayers)) {
		printf("Validation layers requested but not available.\n");
		//cleanup();
	}

	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Hello Triangle",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		//.apiVersion = VK_API_VERSION_1_0,
	};

	unsigned int extensionCount = 0;
	const char **extensions = getRequiredExtensions(enableValidation, &extensionCount);
	VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = 0,
		.enabledExtensionCount = extensionCount,
		.ppEnabledExtensionNames = extensions,
	};

	if(enableValidation) {
		createInfo.enabledLayerCount = sizeof(validationLayers)/sizeof(validationLayers[0]);
		createInfo.ppEnabledLayerNames = validationLayers;
	}

	/*unsigned int extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
	VkExtensionProperties *extensions = malloc(extensionCount*sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);

	for (int i = 0; i < sizeof(extensions)/sizeof(extensions[0]); i++) {
		printf("%s\n", extensions[i].extensionName);
	}*/

	if(vkCreateInstance(&createInfo, NULL, instance) != VK_SUCCESS) {
		printf("Failed to create vulkan instance.\n");
	}
}

void createSurface(VkInstance instance, VkSurfaceKHR *surface, GLFWwindow *window) {
	if(glfwCreateWindowSurface(instance, window, NULL, surface) != VK_SUCCESS) {
		printf("Failed to create surface.\n");
		//cleanup();
	}
}