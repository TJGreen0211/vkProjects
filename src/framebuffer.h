#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <windows.h>

typedef struct fbo {
	unsigned int swapchainImageCount;
	int width;
	int height;
	VkRenderPass renderPass;
	VkImageView depthImageView;
	VkImageView *swapChainImageViews;
} fbo;

void createFramebuffers();

#endif