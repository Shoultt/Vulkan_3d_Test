#pragma once
#include "initVulkan.h"
#include "Control.h"

void updateMVP()
{
	float time = glfwGetTime();
	std::cout << glfwGetTime() << std::endl;

	glm::mat4 model = glm::translate(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)), glm::radians(10.0f) * time, glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.0f, 0.0, 0.0f) + pos);
	glm::mat4 view = glm::lookAt(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 projection = glm::perspective(glm::radians(50.0f), Wwidth / (float)Wheight, 0.1f, 5.0f);
	projection[1][1] *= -1;
	MVP = projection * view * model;
	memcpy(uniformBufferAllocInfo.pMappedData, &MVP, sizeof(MVP));
}

void recordCmdBuffer(VkCommandBuffer cmdBuffer, uint32_t ImageIndex)
{
	VkCommandBufferBeginInfo cbbi = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };

	vkBeginCommandBuffer(cmdBuffer, &cbbi);
	{
		VkRenderPassBeginInfo rpbi = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		rpbi.renderPass = renPass;
		rpbi.framebuffer = frameBuffers[ImageIndex];
		rpbi.renderArea.offset = { 0, 0 };
		rpbi.renderArea.extent = { Wwidth, Wheight };
		VkClearValue colorValue = { 0.20f, 0.07f, 0.48f, 1.0f };
		VkClearValue depthValue = { 1.0f, 0.0f };
		std::vector<VkClearValue> clearValues = { colorValue, depthValue };
		rpbi.clearValueCount = clearValues.size();
		rpbi.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(cmdBuffer, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
		{
			vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffer, offsets);
			vkCmdBindIndexBuffer(cmdBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
			vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphPipeline);
			vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeLayout, 0, 1, &descriptorSet, 0, nullptr);
			vkCmdDrawIndexed(cmdBuffer, (uint32_t)indices.size(), 1, 0, 0, 0);
		}
		vkCmdEndRenderPass(cmdBuffer);
	}
	vkEndCommandBuffer(cmdBuffer);
}

void drawFrame()
{
	vkWaitForFences(device, 1, &checkFence,VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &checkFence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, waitSemaphore, VK_NULL_HANDLE, &imageIndex);

	vkResetCommandBuffer(cmdBuffer, 0);
	recordCmdBuffer(cmdBuffer, imageIndex);

	VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &waitSemaphore;
	submitInfo.pWaitDstStageMask = &waitStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &signalSemaphore;

	vkQueueSubmit(queue, 1, &submitInfo, checkFence);

	VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &signalSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(queue, &presentInfo);
}

void loop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		control();
		updateMVP();
		drawFrame();
	}
}