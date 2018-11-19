#include "window.h"

GLFWwindow *initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	GLFWwindow *window = glfwCreateWindow(400, 400, "Vulkan", NULL, NULL);

	//glfwSetWindowSizeCallback(window, onWindowResized);

	return window;
}

//static void onWindowResized(GLFWwindow *window, int width, int height) {
//	recreateSwapChain();
//}

int getWindowHeight() {
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return mode->height;
}

int getWindowWidth() {
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return mode->width;
}