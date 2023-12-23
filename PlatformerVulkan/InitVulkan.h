#pragma once
#include "VulkanMonotony.h"

class initVK
{
public:
	void InitVulkan()
	{
		createInstance();
		EnumPhysDevice();
		createDevice();
		createSurface();
		createSwapchain();
		swapchainImages();
		createRenderPass();
		createDescriptorSetUboLayout();
		createGraphPipelineLayout();
		createGraphPipeline();
		createAllocator();
		createCommandPool();
		createDepthBuffering();
		createFrameBuffers();
		createVertexBuffer();
		createIndexBuffer();
		createUniformBuffer();
		createImage();
		createDescriptorPool();
		createDescriptorSet();
		allocateCommandBuffer();
		createSynchrone();
	}

	void createInstance()
	{
		VkApplicationInfo ai = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
		ai.pApplicationName = "Platformer";
		ai.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
		ai.pEngineName = "Elf";
		ai.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		ai.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo ici = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		ici.pApplicationInfo = &ai;
		if (enableValidationLayers == true)
		{
			ici.enabledLayerCount = layers.size();
			ici.ppEnabledLayerNames = layers.data();
		}
		else
		{
			ici.enabledLayerCount = 0;
			ici.ppEnabledLayerNames = nullptr;
		}
		ici.enabledExtensionCount = ExtsIns.size();
		ici.ppEnabledExtensionNames = ExtsIns.data();

		vkCreateInstance(&ici, nullptr, &instance);
	}

	void EnumPhysDevice()
	{
		uint32_t physDeviceCount;
		vkEnumeratePhysicalDevices(instance, &physDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> physDevices(physDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physDeviceCount, physDevices.data());
		physDevice = physDevices[0];

		vkGetPhysicalDeviceProperties(physDevice, &physDevProps);
	}

	void createDevice()
	{
		float priorQueue = { 1.0 };
		VkDeviceQueueCreateInfo dqci = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		dqci.queueFamilyIndex = 0;
		dqci.queueCount = 1;
		dqci.pQueuePriorities = &priorQueue;

		VkPhysicalDeviceFeatures features{};
		features.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo dci = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		dci.queueCreateInfoCount = 1;
		dci.pQueueCreateInfos = &dqci;
		dci.enabledExtensionCount = ExtDev.size();
		dci.ppEnabledExtensionNames = ExtDev.data();
		dci.pEnabledFeatures = &features;

		vkCreateDevice(physDevice, &dci, nullptr, &device);

		vkGetDeviceQueue(device, 0, 0, &queue);
	}

	void createSurface()
	{
		glfwCreateWindowSurface(instance, window, nullptr, &surface);
	}

	void createSwapchain()
	{
		VkSwapchainCreateInfoKHR swapCI = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		swapCI.surface = surface;
		swapCI.minImageCount = 2;
		swapCI.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
		swapCI.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		swapCI.imageExtent = VkExtent2D{ Wwidth, Wheight };
		swapCI.imageArrayLayers = 1;
		swapCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapCI.queueFamilyIndexCount = 1;
		swapCI.pQueueFamilyIndices = 0;
		swapCI.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapCI.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		swapCI.clipped = VK_TRUE;
		swapCI.oldSwapchain = VK_NULL_HANDLE;

		vkCreateSwapchainKHR(device, &swapCI, nullptr, &swapchain);
	}

	void swapchainImages()
	{
		uint32_t swapchainImageCount;
		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
		swapImages.resize(swapchainImageCount);
		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapImages.data());

		swapImageViews.resize(swapchainImageCount);

		for (int i = 0; i < swapchainImageCount; i++)
		{
			swapImageViews[i] = createImageViews(swapImages[i], VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void createRenderPass()
	{
		VkAttachmentDescription colorAD{}, depthAD{};
		colorAD.format = VK_FORMAT_B8G8R8A8_UNORM;
		colorAD.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAD.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAD.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAD.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAD.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAD.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAD.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		depthAD.format = VK_FORMAT_D24_UNORM_S8_UINT;
		depthAD.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAD.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAD.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAD.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAD.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAD.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAD.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		std::vector<VkAttachmentDescription> ADs = { colorAD, depthAD };

		VkAttachmentReference colorAR{}, depthAR{};
		colorAR.attachment = 0;
		colorAR.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		depthAR.attachment = 1;
		depthAR.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDependency subDependency{};
		subDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subDependency.dstSubpass = 0;
		subDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subDependency.srcAccessMask = 0;
		subDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		subDependency.dependencyFlags = 0;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAR;
		subpass.pDepthStencilAttachment = &depthAR;

		VkRenderPassCreateInfo renPassCI = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renPassCI.attachmentCount = ADs.size();
		renPassCI.pAttachments = ADs.data();
		renPassCI.subpassCount = 1;
		renPassCI.pSubpasses = &subpass;
		renPassCI.dependencyCount = 1;
		renPassCI.pDependencies = &subDependency;

		vkCreateRenderPass(device, &renPassCI, nullptr, &renPass);
	}

	void createDescriptorSetUboLayout()
	{
		VkDescriptorSetLayoutBinding dslb1{}, dslb2{};
		dslb1.binding = 0;
		dslb1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		dslb1.descriptorCount = 1;
		dslb1.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		dslb2.binding = 1;
		dslb2.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dslb2.descriptorCount = 1;
		dslb2.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::vector<VkDescriptorSetLayoutBinding> dslbS(2); dslbS = { dslb1, dslb2 };

		VkDescriptorSetLayoutCreateInfo descriptSetLayoutCI = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		descriptSetLayoutCI.bindingCount = dslbS.size();
		descriptSetLayoutCI.pBindings = dslbS.data();

		vkCreateDescriptorSetLayout(device, &descriptSetLayoutCI, nullptr, &descriptSetLayout);
	}

	void createGraphPipelineLayout()
	{
		VkPipelineLayoutCreateInfo plci = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		plci.setLayoutCount = 1;
		plci.pSetLayouts = &descriptSetLayout;
		vkCreatePipelineLayout(device, &plci, nullptr, &pipeLayout);
	}

	void createGraphPipeline()
	{
		createShaderModeles(vsCode, &vsModule);
		createShaderModeles(fsCode, &fsModule);
		VkPipelineShaderStageCreateInfo pssciV = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		pssciV.stage = VK_SHADER_STAGE_VERTEX_BIT;
		pssciV.module = vsModule;
		pssciV.pName = "main";
		VkPipelineShaderStageCreateInfo pssciF = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		pssciF.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		pssciF.module = fsModule;
		pssciF.pName = "main";
		VkPipelineShaderStageCreateInfo pssci[] = { pssciV, pssciF };

		std::vector<VkVertexInputBindingDescription> vertexBindingDescrip = getVertexBindingDescrip();
		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescript = getVertexAttributeDescrips();

		VkPipelineVertexInputStateCreateInfo pvisci = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		pvisci.vertexBindingDescriptionCount = 1;
		pvisci.pVertexBindingDescriptions = vertexBindingDescrip.data();
		pvisci.vertexAttributeDescriptionCount = vertexAttributeDescript.size();
		pvisci.pVertexAttributeDescriptions = vertexAttributeDescript.data();

		VkPipelineInputAssemblyStateCreateInfo piasci = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		piasci.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = Wwidth;
		viewport.height = Wheight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor;
		scissor.offset = {0, 0};
		scissor.extent = { Wwidth, Wheight };

		VkPipelineViewportStateCreateInfo pvsci = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		pvsci.viewportCount = 1;
		pvsci.pViewports = &viewport;
		pvsci.scissorCount = 1;
		pvsci.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo prsci = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		prsci.polygonMode = VK_POLYGON_MODE_FILL;
		prsci.cullMode = VK_CULL_MODE_FRONT_BIT;
		prsci.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		prsci.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo pmsci = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		pmsci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pmsci.minSampleShading = 1.0f;

		VkPipelineDepthStencilStateCreateInfo pdsci = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		pdsci.depthTestEnable = VK_TRUE;
		pdsci.depthWriteEnable = VK_TRUE;
		pdsci.depthCompareOp = VK_COMPARE_OP_LESS;
		pdsci.depthBoundsTestEnable = VK_FALSE;
		pdsci.stencilTestEnable = VK_FALSE;
		pdsci.minDepthBounds = 0.0f;
		pdsci.maxDepthBounds = 1.0f;

		VkPipelineColorBlendAttachmentState pcbas{};
		pcbas.blendEnable = VK_FALSE;
		pcbas.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo pcbsci = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		pcbsci.attachmentCount = 1;
		pcbsci.pAttachments = &pcbas;

		VkGraphicsPipelineCreateInfo pipeCI = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipeCI.stageCount = 2;
		pipeCI.pStages = pssci;
		pipeCI.pVertexInputState = &pvisci;
		pipeCI.pInputAssemblyState = &piasci;
		pipeCI.pViewportState = &pvsci;
		pipeCI.pRasterizationState = &prsci;
		pipeCI.pMultisampleState = &pmsci;
		pipeCI.pDepthStencilState = &pdsci;
		pipeCI.pColorBlendState = &pcbsci;
		pipeCI.layout = pipeLayout;
		pipeCI.renderPass = renPass;
		pipeCI.subpass = 0;

		vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeCI, nullptr, &graphPipeline);

		vkDestroyShaderModule(device, fsModule, nullptr);
		vkDestroyShaderModule(device, vsModule, nullptr);
	}
	
	void createCommandPool()
	{
		VkCommandPoolCreateInfo cpci = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		cpci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		cpci.queueFamilyIndex = 0;

		vkCreateCommandPool(device, &cpci, nullptr, &cmdPool);
	}

	void createDepthBuffering()
	{
		createDepthImage();
		createDepthImageView();
		createDepthImageLayout();
	}

	void createFrameBuffers()
	{
		frameBuffers.resize(swapImageViews.size());
		for (int i = 0; i < swapImageViews.size(); i++)
		{
			std::vector<VkImageView> attachmentImages;
			attachmentImages.push_back(swapImageViews[i]);
			attachmentImages.push_back(depthImageView);

			VkFramebufferCreateInfo fbci = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			fbci.renderPass = renPass;
			fbci.attachmentCount = attachmentImages.size();
			fbci.pAttachments = attachmentImages.data();
			fbci.width = Wwidth;
			fbci.height = Wheight;
			fbci.layers = 1;

			vkCreateFramebuffer(device, &fbci, nullptr, &frameBuffers[i]);
		}
	}

	void createVertexBuffer()
	{
		VkDeviceSize vertexSize = sizeof(vertices[0]) * vertices.size();
		createBuffer(vertexBuffer, vertexSize, vertices.data(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBufferAlloc);
	}

	void createIndexBuffer()
	{
		VkDeviceSize indexSize = sizeof(indices[0]) * indices.size();
		createBuffer(indexBuffer, indexSize, indices.data(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBufferAlloc);
	}

	void createUniformBuffer()
	{
		VkDeviceSize uniformBufferSize = sizeof(MVP);
		createUnibuffer(uniformBuffer, uniformBufferSize, uniformBufferAlloc, uniformBufferAllocInfo);
	}

	void createImage()
	{
		uploadTexture("Images/luzha.jpg", textureImageAlloc);
		createTextureImageView();
		createSampler();
	}

	void createDescriptorPool()
	{
		std::vector<VkDescriptorPoolSize> decripPoolSizes(2);
		decripPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		decripPoolSizes[0].descriptorCount = 1;
		decripPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		decripPoolSizes[1].descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptPoolCI = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		descriptPoolCI.maxSets = 1;
		descriptPoolCI.poolSizeCount = decripPoolSizes.size();
		descriptPoolCI.pPoolSizes = decripPoolSizes.data();

		vkCreateDescriptorPool(device, &descriptPoolCI, nullptr, &descriptPool);
	}

	void createDescriptorSet()
	{
		VkDescriptorSetAllocateInfo descriptSetAI = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		descriptSetAI.descriptorPool = descriptPool;
		descriptSetAI.descriptorSetCount = 1;
		descriptSetAI.pSetLayouts = &descriptSetLayout;
		vkAllocateDescriptorSets(device, &descriptSetAI, &descriptorSet);

		VkDescriptorBufferInfo descripBI;
		descripBI.buffer = uniformBuffer;
		descripBI.offset = 0;
		descripBI.range = sizeof(MVP);

		VkDescriptorImageInfo descripII;
		descripII.sampler = textureSampler;
		descripII.imageView = textureImageView;
		descripII.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::vector< VkWriteDescriptorSet> descriptSetWrites(2);
		descriptSetWrites[0].sType = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptSetWrites[0].dstSet = descriptorSet;
		descriptSetWrites[0].dstBinding = 0;
		descriptSetWrites[0].descriptorCount = 1;
		descriptSetWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptSetWrites[0].pBufferInfo = &descripBI;
		descriptSetWrites[1].sType = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptSetWrites[1].dstSet = descriptorSet;
		descriptSetWrites[1].dstBinding = 1;
		descriptSetWrites[1].descriptorCount = 1;
		descriptSetWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptSetWrites[1].pImageInfo = &descripII;

		vkUpdateDescriptorSets(device, descriptSetWrites.size(), descriptSetWrites.data(), 0, nullptr);
	}

	void allocateCommandBuffer()
	{
		VkCommandBufferAllocateInfo cbai = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		cbai.commandPool = cmdPool;
		cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cbai.commandBufferCount = 1;

		vkAllocateCommandBuffers(device, &cbai, &cmdBuffer);
	}

	void createSynchrone()
	{
		VkSemaphoreCreateInfo sci = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

		vkCreateSemaphore(device, &sci, nullptr, &waitSemaphore);
		vkCreateSemaphore(device, &sci, nullptr, &signalSemaphore);

		VkFenceCreateInfo fci = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fci.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		vkCreateFence(device, &fci, nullptr, &checkFence);
	}
};