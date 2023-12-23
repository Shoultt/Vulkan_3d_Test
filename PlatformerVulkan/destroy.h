#pragma once
#include "InitVulkan.h"

void destroy()
{
	vkDeviceWaitIdle(device);

	vkDestroyImageView(device, depthImageView, nullptr);
	vmaDestroyImage(allocator, depthImage, depthImageAlloc);
	vkDestroySampler(device, textureSampler, nullptr);
	vkDestroyImageView(device, textureImageView, nullptr);
	vmaDestroyImage(allocator, textureImage, textureImageAlloc);

	vmaDestroyBuffer(allocator, vertexBuffer, vertexBufferAlloc);
	vmaDestroyBuffer(allocator, indexBuffer, indexBufferAlloc);
	vmaDestroyBuffer(allocator, uniformBuffer, uniformBufferAlloc);
	vmaDestroyAllocator(allocator);

	vkDestroySemaphore(device, waitSemaphore, nullptr);
	vkDestroySemaphore(device, signalSemaphore, nullptr);
	vkDestroyFence(device, checkFence, nullptr);
	vkDestroyCommandPool(device, cmdPool, nullptr);
	vkDestroyDescriptorPool(device, descriptPool, nullptr);
	for (int i = 0; i < swapImageViews.size(); i++)
	vkDestroyFramebuffer(device, frameBuffers[i], nullptr);
	vkDestroyPipeline(device, graphPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipeLayout, nullptr);
	vkDestroyDescriptorSetLayout(device, descriptSetLayout, nullptr);
	vkDestroyRenderPass(device, renPass, nullptr);
	for (int i = 0; i < swapImageViews.size(); i++)
	vkDestroyImageView(device, swapImageViews[i], nullptr);
	vkDestroySwapchainKHR(device, swapchain, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}
