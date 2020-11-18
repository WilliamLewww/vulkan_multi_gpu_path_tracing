#include "device.h"

Device::Device(VkPhysicalDevice physicalDevice) {
  this->framesInFlight = 1;
  this->physicalDevice = physicalDevice;

  vkGetPhysicalDeviceProperties(physicalDevice, &this->physicalDeviceProperties);
  vkGetPhysicalDeviceFeatures(physicalDevice, &this->physicalDeviceFeatures);
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &this->physicalDeviceMemoryProperties);
}

Device::~Device() {

}

VkDevice Device::getLogicalDevice() {
  return this->logicalDevice;
}

VkExtent2D Device::getSwapchainExtent() {
  return this->swapchainExtent;
}

VkRenderPass Device::getRenderPass() {
  return this->renderPass;
}

VkPhysicalDeviceMemoryProperties Device::getPhysicalMemoryProperties() {
  return this->physicalDeviceMemoryProperties;
}

uint32_t Device::getSwapchainImageCount() {
  return this->swapchainImageCount;
}

std::vector<VkImage> Device::getSwapchainImageList() {
  return this->swapchainImageList;
}

VkBuffer Device::getCameraUniformBuffer() {
  return this->cameraUniformBuffer;
}

VkBuffer Device::getTransformUniformBuffer() {
  return this->transformUniformBuffer;
}

VkCommandPool Device::getCommandPool() {
  return this->commandPool;
}

VkQueue Device::getComputeQueue() {
  return this->computeQueue;
}

std::vector<VkFramebuffer> Device::getFramebufferList() {
  return this->framebufferList;
}

VkImageView Device::getRayTraceImageView() {
  return this->rayTraceImageView;
}

VkImage Device::getRayTraceImage() {
  return this->rayTraceImage;
}

void Device::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage* image, VkDeviceMemory* imageMemory) {
  VkImageCreateInfo imageCreateInfo = {};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.extent.width = width;
  imageCreateInfo.extent.height = height;
  imageCreateInfo.extent.depth = 1;
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.format = format;
  imageCreateInfo.tiling = tiling;
  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageCreateInfo.usage = usageFlags;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(this->logicalDevice, &imageCreateInfo, NULL, image) != VK_SUCCESS) {
    printf("failed to create image\n");
  }

  VkMemoryRequirements memoryRequirements;
  vkGetImageMemoryRequirements(this->logicalDevice, *image, &memoryRequirements);

  VkMemoryAllocateInfo memoryAllocateInfo = {};
  memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memoryAllocateInfo.allocationSize = memoryRequirements.size;
  
  uint32_t memoryTypeIndex = -1;
  for (int x = 0; x < this->physicalDeviceMemoryProperties.memoryTypeCount; x++) {
    if ((memoryRequirements.memoryTypeBits & (1 << x)) && (this->physicalDeviceMemoryProperties.memoryTypes[x].propertyFlags & propertyFlags) == propertyFlags) {
      memoryTypeIndex = x;
      break;
    }
  }
  memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

  if (vkAllocateMemory(this->logicalDevice, &memoryAllocateInfo, NULL, imageMemory) != VK_SUCCESS) {
    printf("failed to allocate image memory\n");
  }

  vkBindImageMemory(this->logicalDevice, *image, *imageMemory, 0);
}

void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer* buffer, VkDeviceMemory* bufferMemory) {
  VkBufferCreateInfo bufferCreateInfo = {};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = size;
  bufferCreateInfo.usage = usageFlags;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(this->logicalDevice, &bufferCreateInfo, NULL, buffer) != VK_SUCCESS) {
    printf("failed to create buffer\n");
  }

  VkMemoryRequirements memoryRequirements;
  vkGetBufferMemoryRequirements(this->logicalDevice, *buffer, &memoryRequirements);

  VkMemoryAllocateInfo memoryAllocateInfo = {};
  memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memoryAllocateInfo.allocationSize = memoryRequirements.size;

  uint32_t memoryTypeIndex = -1;
  for (int x = 0; x < this->physicalDeviceMemoryProperties.memoryTypeCount; x++) {
    if ((memoryRequirements.memoryTypeBits & (1 << x)) && (this->physicalDeviceMemoryProperties.memoryTypes[x].propertyFlags & propertyFlags) == propertyFlags) {
      memoryTypeIndex = x;
      break;
    }
  }
  memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

  if (vkAllocateMemory(this->logicalDevice, &memoryAllocateInfo, NULL, bufferMemory) != VK_SUCCESS) {
    printf("failed to allocate buffer memory\n");
  }

  vkBindBufferMemory(this->logicalDevice, *buffer, *bufferMemory, 0);
}

void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
  VkCommandBufferAllocateInfo bufferAllocateInfo = {};
  bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  bufferAllocateInfo.commandPool = this->commandPool;
  bufferAllocateInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(this->logicalDevice, &bufferAllocateInfo, &commandBuffer);
  
  VkCommandBufferBeginInfo commandBufferBeginInfo = {};
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  
  vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
  VkBufferCopy bufferCopy = {};
  bufferCopy.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(this->graphicsQueue);

  vkFreeCommandBuffers(this->logicalDevice, this->commandPool, 1, &commandBuffer);
}

void Device::initializeQueues(VkSurfaceKHR surface) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, NULL);

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, queueFamilyPropertiesList.data());

  this->graphicsQueueIndex = -1;
  this->presentQueueIndex = -1;
  this->computeQueueIndex = -1;

  for (int x = 0; x < queueFamilyCount; x++) {
    if (this->graphicsQueueIndex == -1 && queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      this->graphicsQueueIndex = x;
    }

    if (this->computeQueueIndex == -1 && queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_COMPUTE_BIT) {
      this->computeQueueIndex = x;
    }

    VkBool32 isPresentSupported = 0;
    vkGetPhysicalDeviceSurfaceSupportKHR(this->physicalDevice, x, surface, &isPresentSupported);
    
    if (this->presentQueueIndex == -1 && isPresentSupported) {
      this->presentQueueIndex = x;
    }
  
    if (this->graphicsQueueIndex != -1 && this->presentQueueIndex != -1 && this->computeQueueIndex != -1) {
      break;
    }
  }
}

void Device::createLogicalDevice(std::vector<const char*> extensions) {
  float queuePriority = 1.0f;
  uint32_t deviceQueueCreateInfoCount = 3;

  std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoList(deviceQueueCreateInfoCount);

  deviceQueueCreateInfoList[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  deviceQueueCreateInfoList[0].pNext = NULL;
  deviceQueueCreateInfoList[0].flags = 0;
  deviceQueueCreateInfoList[0].queueFamilyIndex = this->graphicsQueueIndex;
  deviceQueueCreateInfoList[0].queueCount = 1;
  deviceQueueCreateInfoList[0].pQueuePriorities = &queuePriority;
 
  deviceQueueCreateInfoList[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  deviceQueueCreateInfoList[1].pNext = NULL;
  deviceQueueCreateInfoList[1].flags = 0;
  deviceQueueCreateInfoList[1].queueFamilyIndex = this->presentQueueIndex;
  deviceQueueCreateInfoList[1].queueCount = 1;
  deviceQueueCreateInfoList[1].pQueuePriorities = &queuePriority;

  deviceQueueCreateInfoList[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  deviceQueueCreateInfoList[2].pNext = NULL;
  deviceQueueCreateInfoList[2].flags = 0;
  deviceQueueCreateInfoList[2].queueFamilyIndex = this->computeQueueIndex;
  deviceQueueCreateInfoList[2].queueCount = 1;
  deviceQueueCreateInfoList[2].pQueuePriorities = &queuePriority;

  VkPhysicalDeviceBufferDeviceAddressFeaturesEXT bufferDeviceAddressFeatures = {};
  bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT;  
  bufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

  VkPhysicalDeviceRayTracingFeaturesKHR rayTracingFeatures = {};
  rayTracingFeatures.pNext = &bufferDeviceAddressFeatures;
  rayTracingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_FEATURES_KHR;
  rayTracingFeatures.rayTracing = VK_TRUE;
  rayTracingFeatures.rayQuery = VK_TRUE;

  VkPhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.geometryShader = VK_TRUE;
  deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;

  VkDeviceCreateInfo deviceCreateInfo = {};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pNext = &rayTracingFeatures;
  deviceCreateInfo.flags = 0;
  deviceCreateInfo.queueCreateInfoCount = deviceQueueCreateInfoCount;
  deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfoList[0];
  deviceCreateInfo.enabledLayerCount = 0;
  deviceCreateInfo.enabledExtensionCount = extensions.size();
  deviceCreateInfo.ppEnabledExtensionNames = &extensions[0];
  deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

  if (vkCreateDevice(this->physicalDevice, &deviceCreateInfo, NULL, &this->logicalDevice) != VK_SUCCESS) {
    printf("failed to created logical connection to device\n");
  }

  vkGetDeviceQueue(this->logicalDevice, this->graphicsQueueIndex, 0, &this->graphicsQueue);
  vkGetDeviceQueue(this->logicalDevice, this->presentQueueIndex, 0, &this->presentQueue);
  vkGetDeviceQueue(this->logicalDevice, this->computeQueueIndex, 0, &this->computeQueue);
}

void Device::createCommandPool() {
  VkCommandPoolCreateInfo commandPoolCreateInfo = {};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.queueFamilyIndex = this->graphicsQueueIndex;

  if (vkCreateCommandPool(this->logicalDevice, &commandPoolCreateInfo, NULL, &this->commandPool) != VK_SUCCESS) {
    printf("failed to create command pool\n");
  }
}

void Device::createSwapchain(VkSurfaceKHR surface) {
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->physicalDevice, surface, &surfaceCapabilities);
  
  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice, surface, &formatCount, NULL);
  std::vector<VkSurfaceFormatKHR> surfaceFormatList(formatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(this->physicalDevice, surface, &formatCount, surfaceFormatList.data());

  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(this->physicalDevice, surface, &presentModeCount, NULL);
  std::vector<VkPresentModeKHR> presentModeList(presentModeCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(this->physicalDevice, surface, &presentModeCount, presentModeList.data());

  VkSurfaceFormatKHR surfaceFormat = surfaceFormatList[0];
  VkPresentModeKHR presentMode = presentModeList[0];
  VkExtent2D extent = surfaceCapabilities.currentExtent;

  this->swapchainImageCount = surfaceCapabilities.minImageCount + 1;
  if (surfaceCapabilities.maxImageCount > 0 && this->swapchainImageCount > surfaceCapabilities.maxImageCount) {
    this->swapchainImageCount = surfaceCapabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface = surface;
  swapchainCreateInfo.minImageCount = this->swapchainImageCount;
  swapchainCreateInfo.imageFormat = surfaceFormat.format;
  swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapchainCreateInfo.imageExtent = extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

  if (this->graphicsQueueIndex != this->presentQueueIndex) {
    uint32_t queueFamilyIndices[2] = {this->graphicsQueueIndex, this->presentQueueIndex};

    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainCreateInfo.queueFamilyIndexCount = 2;
    swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else {
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = presentMode;
  swapchainCreateInfo.clipped = VK_TRUE;
  swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(this->logicalDevice, &swapchainCreateInfo, NULL, &this->swapchain) != VK_SUCCESS) {
    printf("failed to create swapchain\n");
  }

  vkGetSwapchainImagesKHR(this->logicalDevice, this->swapchain, &this->swapchainImageCount, NULL);
  this->swapchainImageList.resize(this->swapchainImageCount);
  vkGetSwapchainImagesKHR(this->logicalDevice, this->swapchain, &this->swapchainImageCount, this->swapchainImageList.data());

  this->swapchainImageFormat = surfaceFormat.format;
  this->swapchainExtent = extent;

  this->swapchainImageViewList.resize(this->swapchainImageCount);

  for (int x = 0; x < this->swapchainImageCount; x++) {
    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = this->swapchainImageList[x];
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = this->swapchainImageFormat;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(this->logicalDevice, &imageViewCreateInfo, NULL, &this->swapchainImageViewList[x]) != VK_SUCCESS) {
      printf("failed to create image view #%d\n", x);
    }
  }
}

void Device::createRenderPass() {
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = this->swapchainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format = VK_FORMAT_D32_SFLOAT;
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment};

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 2;
  renderPassInfo.pAttachments = attachments;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(this->logicalDevice, &renderPassInfo, NULL, &this->renderPass) != VK_SUCCESS) {
    printf("failed to create render pass\n");
  }
}

void Device::createDepthResource() {
  VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

  createImage(this->swapchainExtent.width, this->swapchainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->depthImage, &this->depthImageMemory);

  VkImageViewCreateInfo viewInfo = {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = this->depthImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = depthFormat;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(this->logicalDevice, &viewInfo, NULL, &this->depthImageView) != VK_SUCCESS) {
    printf("failed to create texture image view\n");
  }
}

void Device::createFramebuffers() {
  this->framebufferList.resize(this->swapchainImageCount);

  for (int x = 0; x < this->swapchainImageCount; x++) {
    VkImageView attachments[2] = {
      this->swapchainImageViewList[x],
      this->depthImageView
    };

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = this->renderPass;
    framebufferCreateInfo.attachmentCount = 2;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = this->swapchainExtent.width;
    framebufferCreateInfo.height = this->swapchainExtent.height;
    framebufferCreateInfo.layers = 1;

    if (vkCreateFramebuffer(this->logicalDevice, &framebufferCreateInfo, NULL, &this->framebufferList[x]) != VK_SUCCESS) {
      printf("failed to create swapchain framebuffer #%d\n", x);
    }
  }
}

void Device::createTextures() {
  createImage(800, 600, this->swapchainImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->rayTraceImage, &this->rayTraceImageMemory);

  VkImageSubresourceRange subresourceRange = {};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;

  VkImageViewCreateInfo imageViewCreateInfo = {};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.pNext = NULL;
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.format = this->swapchainImageFormat;
  imageViewCreateInfo.subresourceRange = subresourceRange;
  imageViewCreateInfo.image = this->rayTraceImage;

  if (vkCreateImageView(this->logicalDevice, &imageViewCreateInfo, NULL, &this->rayTraceImageView) != VK_SUCCESS) {
    printf("failed to create image view\n");
  }

  VkImageMemoryBarrier imageMemoryBarrier = {};
  imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  imageMemoryBarrier.pNext = NULL;
  imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
  imageMemoryBarrier.image = this->rayTraceImage;
  imageMemoryBarrier.subresourceRange = subresourceRange;
  imageMemoryBarrier.srcAccessMask = 0;
  imageMemoryBarrier.dstAccessMask = 0;

  VkCommandBufferAllocateInfo bufferAllocateInfo = {};
  bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  bufferAllocateInfo.commandPool = this->commandPool;
  bufferAllocateInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(this->logicalDevice, &bufferAllocateInfo, &commandBuffer);
  
  VkCommandBufferBeginInfo commandBufferBeginInfo = {};
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  
  vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
  vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(this->computeQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(this->computeQueue);

  vkFreeCommandBuffers(this->logicalDevice, this->commandPool, 1, &commandBuffer);
}

void Device::createUniformBuffers(uint32_t instanceCount, std::vector<uint32_t> vertexOffsetList, std::vector<uint32_t> indexOffsetList, std::vector<float> totalTransformList) {
  VkDeviceSize cameraBufferSize = sizeof(CameraUniform);
  createBuffer(cameraBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &this->cameraUniformBuffer, &this->cameraUniformBufferMemory);

  float* test = (float*)malloc(512);
  memcpy(test, &instanceCount, sizeof(uint32_t));
  memcpy(4 + test, vertexOffsetList.data(), sizeof(uint32_t) * vertexOffsetList.size());
  memcpy(36 + test, indexOffsetList.data(), sizeof(uint32_t) * indexOffsetList.size());
  memcpy(68 + test, totalTransformList.data(), sizeof(float) * totalTransformList.size());

  VkDeviceSize transformBufferSize = 512;
  createBuffer(transformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &this->transformUniformBuffer, &this->transformUniformBufferMemory);

  void* data;
  vkMapMemory(this->logicalDevice, this->transformUniformBufferMemory, 0, transformBufferSize, 0, &data);
  memcpy(data, test, transformBufferSize);
  vkUnmapMemory(this->logicalDevice, this->transformUniformBufferMemory);
}

void Device::createSynchronizationObjects() {
  this->imageAvailableSemaphoreList.resize(this->framesInFlight);
  this->renderFinishedSemaphoreList.resize(this->framesInFlight);
  this->inFlightFenceList.resize(this->framesInFlight);
  this->imageInFlightList.resize(this->swapchainImageCount);
  for (int x = 0; x < this->swapchainImageCount; x++) {
    this->imageInFlightList[x] = VK_NULL_HANDLE;
  }

  VkSemaphoreCreateInfo semaphoreCreateInfo = {};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceCreateInfo = {};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (int x = 0; x < this->framesInFlight; x++) {
    if (vkCreateSemaphore(this->logicalDevice, &semaphoreCreateInfo, NULL, &this->imageAvailableSemaphoreList[x]) == VK_SUCCESS &&
        vkCreateSemaphore(this->logicalDevice, &semaphoreCreateInfo, NULL, &this->renderFinishedSemaphoreList[x]) == VK_SUCCESS &&
        vkCreateFence(this->logicalDevice, &fenceCreateInfo, NULL, &this->inFlightFenceList[x]) != VK_SUCCESS) {
      printf("failed to create synchronization objects for frame #%d\n", x);
    }
  }
}

void Device::updateCameraUniformBuffer(CameraUniform camera) {
  void* data;
  vkMapMemory(this->logicalDevice, this->cameraUniformBufferMemory, 0, sizeof(CameraUniform), 0, &data);
  memcpy(data, &camera, sizeof(CameraUniform));
  vkUnmapMemory(this->logicalDevice, this->cameraUniformBufferMemory);
}

void Device::drawFrame(CameraUniform camera, std::vector<VkCommandBuffer> commandBufferList) {
  vkWaitForFences(this->logicalDevice, 1, &this->inFlightFenceList[this->currentFrame], VK_TRUE, UINT64_MAX);
    
  uint32_t imageIndex;
  vkAcquireNextImageKHR(this->logicalDevice, this->swapchain, UINT64_MAX, this->imageAvailableSemaphoreList[this->currentFrame], VK_NULL_HANDLE, &imageIndex);
    
  if (this->imageInFlightList[imageIndex] != VK_NULL_HANDLE) {
    vkWaitForFences(this->logicalDevice, 1, &this->imageInFlightList[imageIndex], VK_TRUE, UINT64_MAX);
  }
  this->imageInFlightList[imageIndex] = this->inFlightFenceList[this->currentFrame];
 
  updateCameraUniformBuffer(camera);
   
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
  VkSemaphore waitSemaphores[1] = {this->imageAvailableSemaphoreList[this->currentFrame]};
  VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBufferList[imageIndex];

  VkSemaphore signalSemaphores[1] = {this->renderFinishedSemaphoreList[this->currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  vkResetFences(this->logicalDevice, 1, &this->inFlightFenceList[this->currentFrame]);

  if (vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, this->inFlightFenceList[this->currentFrame]) != VK_SUCCESS) {
    printf("failed to submit draw command buffer\n");
  }

  VkPresentInfoKHR presentInfo = {};  
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapchains[1] = {this->swapchain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(this->presentQueue, &presentInfo);

  this->currentFrame = (this->currentFrame + 1) % this->framesInFlight;
}