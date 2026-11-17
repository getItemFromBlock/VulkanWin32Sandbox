#include "RenderThread_Test.hpp"

#include "Resource/Texture.hpp"

#include "renderdoc_app.h"

#include <filesystem>
#include <time.h>
#include <fstream>

using namespace Maths;

const char *alphaBitStringsT[] =
{
	"VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR",
	"VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR",
	"VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR",
	"VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR"
};

RENDERDOC_API_1_1_2 *rdoc_api = NULL;

std::string LoadFileT(const std::string &path)
{
	std::ifstream file = std::ifstream(path, std::ios_base::binary | std::ios_base::ate);
	if (!file.is_open())
		return "";

	u64 size = file.tellg();
	file.seekg(std::ios_base::beg);

	std::string result(size, '0');
	file.read(result.data(), size);
	file.close();

	return result;
}

void RenderThread_Test::Init(HWND hwnd, HINSTANCE hinstance, GameThread *gm, Maths::IVec2 resIn, u32 targetDevice)
{
	appData.hWnd = hwnd;
	appData.hInstance = hinstance;
	appData.gm = gm;
	res = resIn;
	thread = std::thread(&RenderThread_Test::ThreadFunc, this, targetDevice);
}

void RenderThread_Test::Resize(s32 x, s32 y)
{
	u64 packed = (u32)(x) | ((u64)(y) << 32);
	if (packed != lastRes)
		resized = true;
	lastRes = packed;
	storedRes = packed;
}

bool RenderThread_Test::HasFinished() const
{
	return exit;
}

bool RenderThread_Test::HasCrashed() const
{
	return crashed;
}

void RenderThread_Test::Quit()
{
	exit = true;
	if (thread.joinable())
		thread.join();
}

void RenderThread_Test::HandleResize()
{
	res.x = (s32)(storedRes & 0xffffffff);
	res.y = (s32)(storedRes >> 32);
	if (res.x != swapRes.x || res.y != swapRes.y)
		resized = true;
}

void RenderThread_Test::InitThread()
{
	SetThreadDescription(GetCurrentThread(), L"Render Thread Test");
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	start = now.time_since_epoch();
}

void RenderThread_Test::ThreadFunc(u32 targetDevice)
{
	InitThread();
	LoadAssets();

	if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI =
			(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api);
		assert(ret == 1);
	}

	if (!InitVulkan(targetDevice))
	{
		crashed = true;
		return;
	}

	u32 counter = 0;
	u32 tm0 = 0;
	while (!exit)
	{
		std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch() - start;
		auto micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
		f64 iTime = micros / 1000000.0;
		f32 deltaTime = static_cast<f32>(iTime - appTime);
		appTime = iTime;
		u32 tm1 = (u32)(iTime);
		if (tm0 != tm1)
		{
			tm0 = tm1;
			GameThread::LogMessage("FPS: " + std::to_string(counter) + "\n");
			counter = 0;
		}
		counter++;
		
		HandleResize();

		if (!DrawFrame())
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	appData.disp.deviceWaitIdle();
	UnloadAssets();
	Cleanup();

	if (!exit)
		crashed = true;
}

bool RenderThread_Test::InitVulkan(u32 targetDevice)
{
	return	InitDevice(targetDevice) &&
		GetQueues() &&
		CreateRenderPass() &&
		CreateDescriptorSetLayouts() &&
		CreateGraphicsPipeline() &&
		CreateCommandPool() &&
		CreateTextureImage() &&
		CreateTextureImageView() &&
		CreateFramebuffers() &&
		CreateTextureSampler() &&
		CreateObjectBuffers(OBJECT_COUNT) &&
		CreateDescriptorPool() &&
		CreateDescriptorSets() &&
		CreateSyncObjects() &&
		CreateCommandBuffers();
}

void RenderThread_Test::LoadAssets()
{
	sceneData.mesh.CreateDefaultCube();
}

void RenderThread_Test::UnloadAssets()
{
	/*
	kernels.UnloadTextures(textures);
	kernels.UnloadCubemaps(cubemaps);
	kernels.UnloadMaterials();
	kernels.UnloadMeshes(meshes);
	kernels.ClearKernels();
	*/
}

bool RenderThread_Test::InitDevice(u32 targetDevice)
{
	GameThread::LogMessage("Initializing Vulkan...\n");
	auto systemInfoRes = vkb::SystemInfo::get_system_info();
	if (!systemInfoRes)
	{
		GameThread::SendErrorPopup("Error creating vulkan instance: " + systemInfoRes.error().message());
		return false;
	}
	auto &systemInfo = systemInfoRes.value();
	GameThread::LogMessage("Available extensions:\n");
	for (u32 i = 0; i < systemInfo.available_extensions.size(); i++)
		GameThread::LogMessage(std::string("- ") + systemInfo.available_extensions[i].extensionName + "\n");

	vkb::InstanceBuilder instanceBuilder;
	instanceBuilder.enable_extension(VK_KHR_SURFACE_EXTENSION_NAME);
	instanceBuilder.enable_extension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	instanceBuilder.set_app_name("Vulkan Demo").set_app_version(VK_MAKE_VERSION(1, 4, 0));
	instanceBuilder.set_engine_name("Ligma Engine").request_validation_layers();

	instanceBuilder.set_debug_callback([](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void*) -> VkBool32 {
			if (messageSeverity & (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT))
			{
				const char* severity = vkb::to_string_message_severity(messageSeverity);
				const char* type = vkb::to_string_message_type(messageType);
				std::string res = std::string("[") + severity + ": " + type + "] " + pCallbackData->pMessage + "\n";
				OutputDebugStringA(res.c_str());
			}
			// Return false to move on, but return true for validation to skip passing down the call to the driver
			return VK_TRUE;
		});

	auto instanceRet = instanceBuilder.build();

	if (!instanceRet)
	{
		GameThread::SendErrorPopup("Error creating vulkan instance: " + instanceRet.error().message());
		return false;
	}
	appData.instance = instanceRet.value();
	appData.instDisp = appData.instance.make_table();
	//appData.surface = CreateSurfaceWin32(appData.instance, appData.hInstance, appData.hWnd);

	vkb::PhysicalDeviceSelector physDeviceSelector(appData.instance);
	auto devices = physDeviceSelector.require_present(false).select_devices();
	if (!devices)
	{
		std::string err = "No suitable GPU found: " + devices.error().message() + '\n';
		if (devices.error() == vkb::PhysicalDeviceError::no_suitable_device)
		{
			const auto &detailedReasons = devices.detailed_failure_reasons();
			if (!detailedReasons.empty())
			{
				err += "GPU Selection failure reasons:\n";
				for (u32 i = 0; i < detailedReasons.size(); i++)
					err += detailedReasons[i] + '\n';
			}
		}
		GameThread::SendErrorPopup(err);
		return false;
	}
	GameThread::LogMessage("Suitable GPU(s):\n");
	auto &deviceList = devices.value();
	for (u32 i = 0; i < deviceList.size(); i++)
	{
		GameThread::LogMessage(std::to_string(i) + ": " + deviceList[i].name + "\n");
	}
	if (targetDevice >= deviceList.size())
	{
		GameThread::LogMessage("Requested GPU id " + std::to_string(targetDevice) + " is not available. Defaulting to first device.\n");
		targetDevice = 0;
	}
	GameThread::LogMessage("Using GPU device " + std::to_string(targetDevice) + ": " + deviceList[targetDevice].name + "\n");
	vkb::PhysicalDevice physicalDevice = deviceList[targetDevice];
	VkPhysicalDeviceFeatures features = {};
	features.logicOp = VK_TRUE;
	features.samplerAnisotropy = VK_TRUE;
	physicalDevice.enable_features_if_present(features);
	physicalDevice.enable_extension_if_present(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
	vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	VkPhysicalDeviceSynchronization2Features syncFeatures = {};
	syncFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	syncFeatures.synchronization2 = VK_TRUE;
	deviceBuilder.add_pNext<VkPhysicalDeviceSynchronization2Features>(&syncFeatures);

	auto deviceRet = deviceBuilder.build();
	if (!deviceRet)
	{
		GameThread::SendErrorPopup("Error creating device: " + deviceRet.error().message());
		return false;
	}
	appData.device = deviceRet.value();
	appData.disp = appData.device.make_table();
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	appData.maxSamplerAnisotropy = properties.limits.maxSamplerAnisotropy;

	return true;
}

bool RenderThread_Test::GetQueues()
{
	auto gq = appData.device.get_queue(vkb::QueueType::graphics);
	if (!gq.has_value())
	{
		GameThread::SendErrorPopup("failed to get graphics queue: " + gq.error().message());
		return false;
	}
	renderData.graphicsQueue = gq.value();

	auto tq = appData.device.get_queue(vkb::QueueType::transfer);
	if (!tq.has_value())
		renderData.transferQueue = renderData.graphicsQueue;
	else
		renderData.transferQueue = tq.value();
	return true;
}

bool RenderThread_Test::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = nullptr;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (appData.disp.createRenderPass(&renderPassInfo, nullptr, &renderData.renderPass) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to create render pass");
		return false;
	}
	return true;
}

VkShaderModule RenderThread_Test::CreateShaderModule(const std::string &code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const u32*>(code.data());

	VkShaderModule shaderModule;
	if (appData.disp.createShaderModule(&createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		return VK_NULL_HANDLE;

	return shaderModule;
}

bool RenderThread_Test::CreateImage(	IVec2 res, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
								VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &memory)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = (u32)(res.x);
	imageInfo.extent.height = (u32)(res.y);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0; // Optional

	if (appData.disp.createImage(&imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to create image!");
		return false;
	}

	VkMemoryRequirements memRequirements;
	appData.disp.getImageMemoryRequirements(image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (appData.disp.allocateMemory(&allocInfo, nullptr, &memory) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to allocate image memory!");
		return false;
	}

	appData.disp.bindImageMemory(image, memory, 0);
	return true;
}

bool RenderThread_Test::CreateDescriptorSetLayouts()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo layoutInfoRender = {};
	layoutInfoRender.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfoRender.bindingCount = 1;
	VkDescriptorSetLayoutBinding bindings1[1] = { uboLayoutBinding };
	layoutInfoRender.pBindings = bindings1;
	
	if (appData.disp.createDescriptorSetLayout(&layoutInfoRender, nullptr, &renderData.descriptorSetLayoutRender) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to create descriptor set layout");
		return false;
	}

	return true;
}

bool RenderThread_Test::CreateGraphicsPipeline()
{
	const std::filesystem::path defaultPath = std::filesystem::current_path();
	std::string vertCode = LoadFileT(std::filesystem::path(defaultPath).append("Assets/Shaders/simple.vert.spv").string());
	std::string fragCode = LoadFileT(std::filesystem::path(defaultPath).append("Assets/Shaders/dfpwm.frag.spv").string());

	VkShaderModule vertModule = CreateShaderModule(vertCode);
	VkShaderModule fragModule = CreateShaderModule(fragCode);
	if (vertModule == VK_NULL_HANDLE || fragModule == VK_NULL_HANDLE)
	{
		GameThread::SendErrorPopup("failed to create shader module");
		return false;
	}

	VkPipelineShaderStageCreateInfo vertStageInfo = {};
	vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageInfo.module = vertModule;
	vertStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragStageInfo = {};
	fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageInfo.module = fragModule;
	fragStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertStageInfo, fragStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = 4096;
	viewport.height = 2048;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = { 4096, 2048 };

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &renderData.descriptorSetLayoutRender;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	if (appData.disp.createPipelineLayout(&pipelineLayoutInfo, nullptr, &renderData.pipelineLayout) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to create pipeline layout");
		return false;
	}

	std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicInfo = {};
	dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicInfo.dynamicStateCount = static_cast<u32>(dynamicStates.size());
	dynamicInfo.pDynamicStates = dynamicStates.data();

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pDynamicState = &dynamicInfo;
	pipelineInfo.layout = renderData.pipelineLayout;
	pipelineInfo.renderPass = renderData.renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (appData.disp.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &renderData.graphicsPipeline) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to create pipline");
		return false;
	}

	appData.disp.destroyShaderModule(fragModule, nullptr);
	appData.disp.destroyShaderModule(vertModule, nullptr);
	return true;
}

u32 alignT(unsigned int x, unsigned int a)
{
	unsigned int r = x % a;
	return r ? x + (a - r) : x;
}

bool RenderThread_Test::CreateObjectBuffers(u32 objectCount)
{
	VkDeviceSize bufferSizeA = sizeof(Mat4);
	renderData.sizeObjects = alignT(sizeof(Vec4) * 4 * objectCount, 0x40);
	renderData.sizeSortBuf = alignT(SORT_THREAD_COUNT * CHUNK_COUNT * SORT_THREAD_OBJECT_PER_CHUNK * sizeof(u32), 0x40);
	renderData.sizeMergeBuf = alignT(MAX_OBJECTS_PER_CHUNK * CHUNK_COUNT * sizeof(u32), 0x40);
	renderData.mainBufSize = renderData.sizeObjects + renderData.sizeSortBuf + renderData.sizeMergeBuf;
	VkDeviceSize bufferSizeB = renderData.mainBufSize;
	renderData.objectBuffers.resize(3);
	renderData.objectBuffersMemory.resize(3);
	renderData.objectBuffersMapped.resize(3);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSizeB, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	auto sourceData = appData.gm->GetInitialSimulationData();
	void* data;
	appData.disp.mapMemory(stagingBufferMemory, 0, renderData.sizeObjects, 0, &data);
	memcpy(data, sourceData.data(), renderData.sizeObjects);
	appData.disp.unmapMemory(stagingBufferMemory);

	bool success = true;
	for (u32 i = 0; i < 3; i++)
	{
		success &= CreateBuffer(bufferSizeA,
								VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
								VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
								renderData.objectBuffers[i],
								renderData.objectBuffersMemory[i]);
	
		success &= appData.disp.mapMemory(	renderData.objectBuffersMemory[i],
											0,
											bufferSizeA,
											0,
											reinterpret_cast<void**>(&renderData.objectBuffersMapped[i])) == VK_SUCCESS;
	}

	success &= CreateBuffer(bufferSizeB,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		renderData.computeBuffer,
		renderData.computeBufferMemory);

	CopyBuffer(stagingBuffer, renderData.computeBuffer, bufferSizeB);

	appData.disp.destroyBuffer(stagingBuffer, nullptr);
	appData.disp.freeMemory(stagingBufferMemory, nullptr);

	return success;
}

bool RenderThread_Test::CreateFramebuffers()
{
	//renderData.swapchainImages = appData.swapchain.get_images().value();
	//renderData.swapchainImageViews = appData.swapchain.get_image_views().value();

	renderData.framebuffers.resize(1);

	for (u32 i = 0; i < 1; i++)
	{
		//VkImageView attachments[2] = { renderData.swapchainImageViews[i], renderData.depthImageView };
		

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderData.renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &renderData.textureImageView;
		framebufferInfo.width = 4096;
		framebufferInfo.height = 2048;
		framebufferInfo.layers = 1;

		if (appData.disp.createFramebuffer(&framebufferInfo, nullptr, &renderData.framebuffers[i]) != VK_SUCCESS)
			return false;
	}
	return true;
}

bool RenderThread_Test::CreateCommandPool()
{
	VkCommandPoolCreateInfo poolInfo0 = {};
	poolInfo0.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo0.queueFamilyIndex = appData.device.get_queue_index(vkb::QueueType::graphics).value();

	if (appData.disp.createCommandPool(&poolInfo0, nullptr, &renderData.commandPool) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to create command pool");
		return false;
	}

	VkCommandPoolCreateInfo poolInfo1 = {};
	poolInfo1.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	auto transfer = appData.device.get_queue_index(vkb::QueueType::transfer);
	poolInfo1.queueFamilyIndex = transfer.has_value() ? transfer.value() : poolInfo0.queueFamilyIndex;

	if (appData.disp.createCommandPool(&poolInfo1, nullptr, &renderData.transfertCommandPool) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to create command pool");
		return false;
	}
	return true;
}

bool RenderThread_Test::CreateTextureImage()
{

	CreateImage(IVec2(4096, 2048), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderData.textureImage, renderData.textureImageMemory);

	TransitionImageLayout(renderData.textureImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	return true;
}

bool RenderThread_Test::CreateTextureImageView()
{
	renderData.textureImageView = CreateImageView(renderData.textureImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);
	return renderData.textureImageView != nullptr;
}

bool RenderThread_Test::CreateTextureSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_NEAREST;
	samplerInfo.minFilter = VK_FILTER_NEAREST;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = appData.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (appData.disp.createSampler(&samplerInfo, nullptr, &renderData.textureSampler) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to create texture sampler!");
		return false;
	}

	return true;
}

VkImageView RenderThread_Test::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView = nullptr;
	if (vkCreateImageView(appData.device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		GameThread::SendErrorPopup("failed to create image view!");

	return imageView;
}

VkCommandBuffer RenderThread_Test::BeginSingleTimeCommands(VkCommandPool targetPool)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = targetPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	appData.disp.allocateCommandBuffers(&allocInfo, &commandBuffer);
	
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	appData.disp.beginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void RenderThread_Test::EndSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool targetPool, VkQueue targetQueue)
{
	appData.disp.endCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	appData.disp.queueSubmit(targetQueue, 1, &submitInfo, VK_NULL_HANDLE);
	appData.disp.queueWaitIdle(targetQueue);
	appData.disp.freeCommandBuffers(targetPool, 1, &commandBuffer);
}

bool RenderThread_Test::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands(renderData.transfertCommandPool);
	
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	appData.disp.cmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer, renderData.transfertCommandPool, renderData.transferQueue);

	return true;
}

bool RenderThread_Test::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands(renderData.commandPool);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0; // TODO
	barrier.dstAccessMask = 0; // TODO

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else
	{
		GameThread::SendErrorPopup("unsupported layout transition!");
	}

	appData.disp.cmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	EndSingleTimeCommands(commandBuffer, renderData.commandPool, renderData.graphicsQueue);

	return true;
}

void RenderThread_Test::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands(renderData.transfertCommandPool);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = {0, 0, 0};
	region.imageExtent = {
		width,
		height,
		1
	};

	appData.disp.cmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	EndSingleTimeCommands(commandBuffer, renderData.transfertCommandPool, renderData.transferQueue);
}

bool RenderThread_Test::CreateCommandBuffers()
{
	if (rdoc_api) rdoc_api->StartFrameCapture(appData.device , NULL);

	renderData.commandBuffers.resize(renderData.framebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = renderData.commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (u32)renderData.commandBuffers.size();

	if (appData.disp.allocateCommandBuffers(&allocInfo, renderData.commandBuffers.data()) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to allocate command buffers");
		return false;
	}

	VkCommandBufferAllocateInfo allocInfoTr = {};
	allocInfoTr.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfoTr.commandPool = renderData.commandPool;
	allocInfoTr.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfoTr.commandBufferCount = 1;

	if (appData.disp.allocateCommandBuffers(&allocInfoTr, &renderData.transferCommandBuffer) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to allocate transfer command buffer");
		return false;
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(4096 * 2048 * 4 * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	for (u32 i = 0; i < 1; i++)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (appData.disp.beginCommandBuffer(renderData.commandBuffers[0], &beginInfo) != VK_SUCCESS)
		{
			GameThread::SendErrorPopup("failed to begin recording command buffer");
			return false;
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderData.renderPass;
		renderPassInfo.framebuffer = renderData.framebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = {4096, 2048 };
		VkClearValue clearColors[2];
		clearColors[0].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
		clearColors[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearColors;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = 4096;
		viewport.height = 2048;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { 4096, 2048 };


		// Render
		appData.disp.cmdSetViewport(renderData.commandBuffers[0], 0, 1, &viewport);
		appData.disp.cmdSetScissor(renderData.commandBuffers[0], 0, 1, &scissor);

		appData.disp.cmdBeginRenderPass(renderData.commandBuffers[0], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		appData.disp.cmdBindPipeline(renderData.commandBuffers[0], VK_PIPELINE_BIND_POINT_GRAPHICS, renderData.graphicsPipeline);

		VkBuffer vertexBuffers[] = { renderData.vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		//appData.disp.cmdBindVertexBuffers(renderData.commandBuffers[i], 0, 1, vertexBuffers, offsets);

		appData.disp.cmdBindDescriptorSets(renderData.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderData.pipelineLayout, 0, 1, &renderData.descriptorSets[i], 0, nullptr);

		appData.disp.cmdDraw(renderData.commandBuffers[0], 3, 1, 0, 0);

		appData.disp.cmdEndRenderPass(renderData.commandBuffers[0]);

		VkBufferImageCopy imageCopy = {};
		imageCopy.bufferImageHeight = 0;
		imageCopy.bufferOffset = 0;
		imageCopy.bufferRowLength = 0;
		imageCopy.imageExtent = { 4096, 2048, 1 };
		imageCopy.imageOffset = { 0, 0, 0 };
		imageCopy.imageSubresource.mipLevel = 0;
		imageCopy.imageSubresource.layerCount = 1;
		imageCopy.imageSubresource.baseArrayLayer = 0;
		imageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;


		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = renderData.textureImage;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0; // TODO
		barrier.dstAccessMask = 0; // TODO

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

		appData.disp.cmdPipelineBarrier(
			renderData.commandBuffers[0],
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		vkCmdCopyImageToBuffer(renderData.commandBuffers[0], renderData.textureImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer, 1, &imageCopy);

		if (appData.disp.endCommandBuffer(renderData.commandBuffers[0]) != VK_SUCCESS)
		{
			GameThread::SendErrorPopup("failed to record command buffer");
			return false;
		}
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { renderData.availableSemaphores[0] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &renderData.commandBuffers[0];

	appData.disp.resetFences(1, &renderData.inFlightFences[0]);

	UpdateUniformBuffer(renderData.currentFrame);

	if (appData.disp.queueSubmit(renderData.graphicsQueue, 1, &submitInfo, renderData.inFlightFences[0]) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to submit draw command buffer");
		return false;
	}
	vkDeviceWaitIdle(appData.device);

	float *memory = new float[4096 * 2048 * 4];
	void *data;
	appData.disp.mapMemory(stagingBufferMemory, 0, 4096*2048*4*4, 0, &data);
	const float *dataSource = reinterpret_cast<const float *>(data);
	u32 lastNZ = 0;
	for (u32 i = 0; i < 4096*2048; i++)
	{
		if (dataSource[i * 4] != 0 || dataSource[i * 4 + 1] != 0)
			lastNZ = i;
		memory[i * 2] = dataSource[i * 4];
		memory[i * 2 + 1] = dataSource[i * 4 + 1];
	}
	lastNZ = lastNZ + 500 < 4096*2048 ? lastNZ + 500 : 4096 * 2048;
	appData.disp.unmapMemory(stagingBufferMemory);

	std::ofstream output = std::ofstream("output.raw", std::ios_base::binary | std::ios_base::trunc);
	if (output.is_open() && output.good())
	{
		output.write(reinterpret_cast<const char*>(memory), lastNZ * 2 * 4);
	}

	delete[] memory;
	
	if (rdoc_api) rdoc_api->EndFrameCapture(appData.device, NULL);

	return true;
}

bool RenderThread_Test::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	bufferInfo.queueFamilyIndexCount = 2;
	auto transfer = appData.device.get_queue_index(vkb::QueueType::transfer);
	auto graph = appData.device.get_queue_index(vkb::QueueType::graphics);
	u32 queueFamilies[2] = {appData.device.get_queue_index(vkb::QueueType::graphics).value(), transfer.has_value() ? transfer.value() : graph.has_value()};
	bufferInfo.pQueueFamilyIndices = queueFamilies;

	if (appData.disp.createBuffer(&bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to create buffer!");
		return false;
	}

	VkMemoryRequirements memRequirements;
	appData.disp.getBufferMemoryRequirements(buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (appData.disp.allocateMemory(&allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to allocate buffer memory!");
		return false;
	}

	appData.disp.bindBufferMemory(buffer, bufferMemory, 0);
	return true;
}

bool RenderThread_Test::CreateSyncObjects()
{
	renderData.availableSemaphores.resize(3);
	renderData.finishedSemaphore.resize(appData.swapchain.image_count);
	renderData.inFlightFences.resize(3);
	renderData.imageInFlight.resize(appData.swapchain.image_count, VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (u32 i = 0; i < appData.swapchain.image_count; i++)
	{
		if (appData.disp.createSemaphore(&semaphoreInfo, nullptr, &renderData.finishedSemaphore[i]) != VK_SUCCESS)
		{
			GameThread::SendErrorPopup("failed to create sync objects");
			return false;
		}
	}

	for (u32 i = 0; i < 3; i++)
	{
		if (appData.disp.createSemaphore(&semaphoreInfo, nullptr, &renderData.availableSemaphores[i]) != VK_SUCCESS ||
			appData.disp.createFence(&fenceInfo, nullptr, &renderData.inFlightFences[i]) != VK_SUCCESS)
		{
			GameThread::SendErrorPopup("failed to create sync objects");
			return false;
		}
	}
	return true;
}

bool RenderThread_Test::CreateDescriptorPool()
{
	VkDescriptorPoolSize poolSize0 = {};
	poolSize0.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize0.descriptorCount = 256;
	VkDescriptorPoolSize poolSize1 = {};
	poolSize1.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSize1.descriptorCount = 256;
	VkDescriptorPoolSize poolSize2 = {};
	poolSize2.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize2.descriptorCount = 256;

	VkDescriptorPoolSize pools[3] = {poolSize0, poolSize1, poolSize2};
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 3;
	poolInfo.pPoolSizes = pools;
	poolInfo.maxSets = 256;

	VkDescriptorPoolSize poolsCompute[2] = {poolSize1, poolSize1};
	VkDescriptorPoolCreateInfo poolInfoCompute = {};
	poolInfoCompute.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfoCompute.poolSizeCount = 2;
	poolInfoCompute.pPoolSizes = poolsCompute;
	poolInfoCompute.maxSets = 256;


	if (appData.disp.createDescriptorPool(&poolInfo, nullptr, &renderData.descriptorPool) != VK_SUCCESS ||
		appData.disp.createDescriptorPool(&poolInfoCompute, nullptr, &renderData.descriptorPoolCompute) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to create descriptor pool");
		return false;
	}

	return true;
}

VkWriteDescriptorSet RenderThread_Test::CreateWriteDescriptorSet(VkDescriptorSet dstSet, u32 binding, VkDescriptorType type, VkDescriptorBufferInfo * bufferInfo, VkDescriptorImageInfo * imageInfo)
{
	VkWriteDescriptorSet result = {};
	result.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	result.dstSet = dstSet;
	result.dstBinding = binding;
	result.dstArrayElement = 0;
	result.descriptorType = type;
	result.descriptorCount = 1;
	result.pBufferInfo = bufferInfo;
	result.pImageInfo = imageInfo;

	return result;
}

bool RenderThread_Test::CreateDescriptorSets()
{
	//return true;
	std::vector<VkDescriptorSetLayout> layouts(1, renderData.descriptorSetLayoutRender);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = renderData.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts.data();

	renderData.descriptorSets.resize(3);
	if (appData.disp.allocateDescriptorSets(&allocInfo, renderData.descriptorSets.data()) != VK_SUCCESS)
	{
		GameThread::SendErrorPopup("failed to allocate descriptor sets");
		return false;
	}


	for (size_t i = 0; i < 3; i++)
	{
		VkDescriptorBufferInfo bufferInfoUBO = {};
		bufferInfoUBO.buffer = renderData.objectBuffers[i];
		bufferInfoUBO.offset = 0;
		bufferInfoUBO.range = sizeof(Mat4);

		VkWriteDescriptorSet descriptorWriteUBO = CreateWriteDescriptorSet(renderData.descriptorSets[i], 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfoUBO);
		appData.disp.updateDescriptorSets(1, &descriptorWriteUBO, 0, nullptr);
	}

	return true;
}

bool RenderThread_Test::UpdateUniformBuffer(u32 image)
{
	Vec4 *dataPtr = renderData.objectBuffersMapped[image];
	const auto &mat = appData.gm->GetViewProjectionMatrix();
	const Vec4 *matPtr = reinterpret_cast<const Vec4*>(mat.content);
	for (u32 i = 0; i < 4; i++)
	{
		dataPtr[i] = Vec4(0);
	}

	dataPtr[1] = Vec4(0,44100.0f,0,0);

	return true;
}

u32 RenderThread_Test::FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	appData.instDisp.getPhysicalDeviceMemoryProperties(appData.device.physical_device, &memProperties);
	
	for (u32 i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	GameThread::SendErrorPopup("failed to find suitable memory type!");
	return -1;
}

VkFormat RenderThread_Test::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(appData.device.physical_device, format, &props);
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			return format;
	}

	GameThread::SendErrorPopup("failed to find supported format!");
	return VK_FORMAT_UNDEFINED;
}

VkFormat RenderThread_Test::FindDepthFormat()
{
	return FindSupportedFormat(
		{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool RenderThread_Test::DrawFrame()
{
	return true;
}

void RenderThread_Test::Cleanup()
{
	for (u32 i = 0; i < appData.swapchain.image_count; i++)
	{
		appData.disp.destroySemaphore(renderData.finishedSemaphore[i], nullptr);
	}
	for (u32 i = 0; i < 3; i++)
	{
		appData.disp.destroySemaphore(renderData.availableSemaphores[i], nullptr);
		appData.disp.destroyFence(renderData.inFlightFences[i], nullptr);
	}

	appData.disp.destroyCommandPool(renderData.commandPool, nullptr);
	appData.disp.destroyCommandPool(renderData.transfertCommandPool, nullptr);

	for (u32 i = 0; i < renderData.framebuffers.size(); i++)
	{
		appData.disp.destroyFramebuffer(renderData.framebuffers[i], nullptr);
	}

	for (u32 i = 0; i < renderData.objectBuffers.size(); i++)
	{
		appData.disp.destroyBuffer(renderData.objectBuffers[i], nullptr);
		appData.disp.freeMemory(renderData.objectBuffersMemory[i], nullptr);
	}
	appData.disp.destroyBuffer(renderData.computeBuffer, nullptr);
	appData.disp.freeMemory(renderData.computeBufferMemory, nullptr);

	appData.disp.destroyPipeline(renderData.graphicsPipeline, nullptr);
	for (u32 i = 0; i < 4; i++)
	{
		appData.disp.destroyPipeline(renderData.computePipelines[i], nullptr);
	}
	appData.disp.destroyPipelineLayout(renderData.pipelineLayout, nullptr);
	appData.disp.destroyPipelineLayout(renderData.computePipelineLayout, nullptr);
	appData.disp.destroyRenderPass(renderData.renderPass, nullptr);
	appData.disp.destroyBuffer(renderData.vertexBuffer, nullptr);
	appData.disp.destroyDescriptorPool(renderData.descriptorPool, nullptr);
	appData.disp.destroyDescriptorPool(renderData.descriptorPoolCompute, nullptr);
	appData.disp.destroyDescriptorSetLayout(renderData.descriptorSetLayoutRender, nullptr);
	appData.disp.destroyDescriptorSetLayout(renderData.descriptorSetLayoutCompute, nullptr);
	appData.disp.freeMemory(renderData.vertexBufferMemory, nullptr);
	appData.disp.destroySampler(renderData.textureSampler, nullptr);
	appData.disp.destroyImageView(renderData.textureImageView, nullptr);
	appData.disp.destroyImage(renderData.textureImage, nullptr);
	appData.disp.freeMemory(renderData.textureImageMemory, nullptr);

	appData.disp.destroyImageView(renderData.depthImageView, nullptr);
	appData.disp.destroyImage(renderData.depthImage, nullptr);
	appData.disp.freeMemory(renderData.depthImageMemory, nullptr);

	appData.swapchain.destroy_image_views(renderData.swapchainImageViews);

	vkb::destroy_swapchain(appData.swapchain);
	vkb::destroy_device(appData.device);
	vkb::destroy_surface(appData.instance, appData.surface);
	vkb::destroy_instance(appData.instance);
}
