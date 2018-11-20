#include "framebuffer.h"

void createFramebuffers(fbo *framebuffer) {
	swapChainFramebuffers = malloc(framebuffer->swapchainImageCount*sizeof(VkFramebuffer));
	for(unsigned int i = 0; i < framebuffer->swapchainImageCount; i++) {
		VkImageView attachments[2] = {
			swapChainImageViews[i],
			depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = framebuffer->renderPass,
			.attachmentCount = 2,
			.pAttachments = attachments,
			.width = framebuffer->width,
			.height = framebuffer->height,
			.layers = 1,
		};

		if(vkCreateFramebuffer(device, &framebufferInfo, NULL, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			printf("Failed to create framebuffer.\n");
			cleanup();
		}
	}
}
