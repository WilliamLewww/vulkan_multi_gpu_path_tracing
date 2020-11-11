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

VkBuffer Device::getCameraUniformBuffer() {
  return this->cameraUniformBuffer;
}

VkBuffer Device::getTransformUniformBuffer() {
  return this->transformUniformBuffer;
}

VkBuffer Device::getIndexBuffer() {
  return this->indexBuffer;
}

VkBuffer Device::getVertexBuffer() {
  return this->vertexPositionBuffer;
}

VkImageView Device::getRayTraceImageView() {
  return this->rayTraceImageView;
}

VkBuffer Device::getMaterialIndexBuffer() {
  return this->materialIndexBuffer;
}

VkBuffer Device::getMaterialBuffer() {
  return this->materialBuffer;
}

VkBuffer Device::getMaterialLightBuffer() {
  return this->materialLightBuffer;
}

VkCommandPool Device::getCommandPool() {
  return this->commandPool;
}

VkQueue Device::getComputeQueue() {
  return this->computeQueue;
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

  if (vkCreateImage(this->logicalDevice, &imageCreateInfo, NULL, image) == VK_SUCCESS) {
    printf("created image\n");
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
    printf("allocated image memory\n");
  }

  vkBindImageMemory(this->logicalDevice, *image, *imageMemory, 0);
}

void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer* buffer, VkDeviceMemory* bufferMemory) {
  VkBufferCreateInfo bufferCreateInfo = {};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = size;
  bufferCreateInfo.usage = usageFlags;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(this->logicalDevice, &bufferCreateInfo, NULL, buffer) == VK_SUCCESS) {
    printf("created buffer\n");
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

  if (vkAllocateMemory(this->logicalDevice, &memoryAllocateInfo, NULL, bufferMemory) == VK_SUCCESS) {
    printf("allocated buffer memory\n");
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

  if (vkCreateDevice(this->physicalDevice, &deviceCreateInfo, NULL, &this->logicalDevice) == VK_SUCCESS) {
    printf("created logical connection to device\n");
  }

  vkGetDeviceQueue(this->logicalDevice, this->graphicsQueueIndex, 0, &this->graphicsQueue);
  vkGetDeviceQueue(this->logicalDevice, this->presentQueueIndex, 0, &this->presentQueue);
  vkGetDeviceQueue(this->logicalDevice, this->computeQueueIndex, 0, &this->computeQueue);
}

void Device::createCommandPool() {
  VkCommandPoolCreateInfo commandPoolCreateInfo = {};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.queueFamilyIndex = this->graphicsQueueIndex;

  if (vkCreateCommandPool(this->logicalDevice, &commandPoolCreateInfo, NULL, &this->commandPool) == VK_SUCCESS) {
    printf("created command pool\n");
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

  if (vkCreateSwapchainKHR(this->logicalDevice, &swapchainCreateInfo, NULL, &this->swapchain) == VK_SUCCESS) {
    printf("created swapchain\n");
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

    if (vkCreateImageView(this->logicalDevice, &imageViewCreateInfo, NULL, &this->swapchainImageViewList[x]) == VK_SUCCESS) {
      printf("created image view #%d\n", x);
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

  if (vkCreateRenderPass(this->logicalDevice, &renderPassInfo, NULL, &this->renderPass) == VK_SUCCESS) {
    printf("created render pass\n");
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

  if (vkCreateImageView(this->logicalDevice, &viewInfo, NULL, &this->depthImageView) == VK_SUCCESS) {
    printf("created texture image view\n");
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

    if (vkCreateFramebuffer(this->logicalDevice, &framebufferCreateInfo, NULL, &this->framebufferList[x]) == VK_SUCCESS) {
      printf("created swapchain framebuffer #%d\n", x);
    }
  }
}

void Device::createVertexBuffer(Model* model) {
  VkDeviceSize positionBufferSize = sizeof(float) * model->getVertexCount();
  
  VkBuffer positionStagingBuffer;
  VkDeviceMemory positionStagingBufferMemory;
  createBuffer(positionBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &positionStagingBuffer, &positionStagingBufferMemory);

  void* positionData;
  vkMapMemory(this->logicalDevice, positionStagingBufferMemory, 0, positionBufferSize, 0, &positionData);
  memcpy(positionData, model->getVertices().data(), positionBufferSize);
  vkUnmapMemory(this->logicalDevice, positionStagingBufferMemory);

  createBuffer(positionBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->vertexPositionBuffer, &this->vertexPositionBufferMemory);  

  copyBuffer(positionStagingBuffer, this->vertexPositionBuffer, positionBufferSize);

  vkDestroyBuffer(this->logicalDevice, positionStagingBuffer, NULL);
  vkFreeMemory(this->logicalDevice, positionStagingBufferMemory, NULL);
}

void Device::createIndexBuffer(Model* model) {
  VkDeviceSize bufferSize = sizeof(uint32_t) * model->getTotalIndexCount();

  std::vector<uint32_t> positionIndexList(model->getTotalIndexCount());
  for (int x = 0; x < model->getTotalIndexCount(); x++) {
    positionIndexList[x] = model->getTotalIndex(x).vertex_index;
  }
  
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

  void* data;
  vkMapMemory(this->logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, positionIndexList.data(), bufferSize);
  vkUnmapMemory(this->logicalDevice, stagingBufferMemory);

  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->indexBuffer, &this->indexBufferMemory);

  copyBuffer(stagingBuffer, this->indexBuffer, bufferSize);
  
  vkDestroyBuffer(this->logicalDevice, stagingBuffer, NULL);
  vkFreeMemory(this->logicalDevice, stagingBufferMemory, NULL);
}

void Device::createMaterialBuffers(Model* model) {
  VkDeviceSize indexBufferSize = sizeof(uint32_t) * model->getTotalMaterialIndexCount();

  std::vector<int> materialIndexList(model->getTotalMaterialIndexCount());
  for (int x = 0; x < model->getTotalMaterialIndexCount(); x++) {
    materialIndexList[x] = model->getTotalMaterialIndex(x);
  }

  VkBuffer indexStagingBuffer;
  VkDeviceMemory indexStagingBufferMemory;
  createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &indexStagingBuffer, &indexStagingBufferMemory);

  void* indexData;
  vkMapMemory(this->logicalDevice, indexStagingBufferMemory, 0, indexBufferSize, 0, &indexData);
  memcpy(indexData, materialIndexList.data(), indexBufferSize);
  vkUnmapMemory(this->logicalDevice, indexStagingBufferMemory);

  createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->materialIndexBuffer, &this->materialIndexBufferMemory);

  copyBuffer(indexStagingBuffer, this->materialIndexBuffer, indexBufferSize);
  
  vkDestroyBuffer(this->logicalDevice, indexStagingBuffer, NULL);
  vkFreeMemory(this->logicalDevice, indexStagingBufferMemory, NULL);

  VkDeviceSize materialBufferSize = sizeof(struct Material) * model->getMaterialCount();

  std::vector<Material> materialList(model->getMaterialCount());
  for (int x = 0; x < model->getMaterialCount(); x++) {
    memcpy(materialList[x].ambient, model->getMaterial(x).ambient, sizeof(float) * 3);
    memcpy(materialList[x].diffuse, model->getMaterial(x).diffuse, sizeof(float) * 3);
    memcpy(materialList[x].specular, model->getMaterial(x).specular, sizeof(float) * 3);
    memcpy(materialList[x].emission, model->getMaterial(x).emission, sizeof(float) * 3);
  }

  VkBuffer materialStagingBuffer;
  VkDeviceMemory materialStagingBufferMemory;
  createBuffer(materialBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &materialStagingBuffer, &materialStagingBufferMemory);

  void* materialData;
  vkMapMemory(this->logicalDevice, materialStagingBufferMemory, 0, materialBufferSize, 0, &materialData);
  memcpy(materialData, materialList.data(), materialBufferSize);
  vkUnmapMemory(this->logicalDevice, materialStagingBufferMemory);

  createBuffer(materialBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->materialBuffer, &this->materialBufferMemory);

  copyBuffer(materialStagingBuffer, this->materialBuffer, materialBufferSize);
  
  vkDestroyBuffer(this->logicalDevice, materialStagingBuffer, NULL);
  vkFreeMemory(this->logicalDevice, materialStagingBufferMemory, NULL);

  LightContainer lightContainer = {};
  for (int x = 0; x < model->getTotalMaterialIndexCount(); x++) {
    float* materialEmission = model->getMaterial(model->getTotalMaterialIndex(x)).emission;
    if (materialEmission[0] > 0 || materialEmission[1] > 0 || materialEmission[2] > 0) {
      lightContainer.indices[lightContainer.count] = x;
      lightContainer.count += 1;
    }
  }
  
  VkBuffer materialLightStagingBuffer;
  VkDeviceMemory materialLightStagingBufferMemory;
  createBuffer(sizeof(LightContainer), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &materialLightStagingBuffer, &materialLightStagingBufferMemory);

  void* materialLightData;
  vkMapMemory(this->logicalDevice, materialLightStagingBufferMemory, 0, sizeof(LightContainer), 0, &materialLightData);
  memcpy(materialLightData, &lightContainer, sizeof(LightContainer));
  vkUnmapMemory(this->logicalDevice, materialLightStagingBufferMemory);

  createBuffer(sizeof(LightContainer), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->materialLightBuffer, &this->materialLightBufferMemory);

  copyBuffer(materialLightStagingBuffer, this->materialLightBuffer, sizeof(LightContainer));
  
  vkDestroyBuffer(this->logicalDevice, materialLightStagingBuffer, NULL);
  vkFreeMemory(this->logicalDevice, materialLightStagingBufferMemory, NULL);
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

  if (vkCreateImageView(this->logicalDevice, &imageViewCreateInfo, NULL, &this->rayTraceImageView) == VK_SUCCESS) {
    printf("created image view\n");
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

void Device::createUniformBuffers() {
  VkDeviceSize cameraBufferSize = sizeof(CameraUniform);
  createBuffer(cameraBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &this->cameraUniformBuffer, &this->cameraUniformBufferMemory);

  VkDeviceSize transformBufferSize = sizeof(float) * 16;
  createBuffer(transformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &this->transformUniformBuffer, &this->transformUniformBufferMemory);

  float transform[16] = {
    1.0, 0, 0, 0,
    0, 1.0, 0, 0,
    0, 0, 1.0, 0,
    0, 0, 0, 1.0,
  };

  void* data;
  vkMapMemory(this->logicalDevice, this->transformUniformBufferMemory, 0, transformBufferSize, 0, &data);
  memcpy(data, transform, transformBufferSize);
  vkUnmapMemory(this->logicalDevice, this->transformUniformBufferMemory);
}

void Device::createCommandBuffers(Model* model, VkPipeline pipeline, VkPipelineLayout pipelineLayout, std::vector<VkDescriptorSet>& descriptorSetList) {
  this->commandBufferList.resize(this->swapchainImageCount);
  
  VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandPool = this->commandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandBufferCount = this->swapchainImageCount;

  if (vkAllocateCommandBuffers(this->logicalDevice, &commandBufferAllocateInfo, this->commandBufferList.data()) == VK_SUCCESS) {
    printf("allocated command buffers\n");
  }

  for (int x = 0; x < this->swapchainImageCount; x++) {
    VkCommandBufferBeginInfo commandBufferBeginCreateInfo = {};
    commandBufferBeginCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = this->renderPass;
    renderPassBeginInfo.framebuffer = this->framebufferList[x];
    VkOffset2D renderAreaOffset = {0, 0};
    renderPassBeginInfo.renderArea.offset = renderAreaOffset;
    renderPassBeginInfo.renderArea.extent = this->swapchainExtent;

    VkClearValue clearValues[2] = {
      {.color = {0.0f, 0.0f, 0.0f, 1.0f}},
      {.depthStencil = {1.0f, 0}}
    };

    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    VkBuffer vertexBuffers[1] = {this->vertexPositionBuffer};
    VkDeviceSize offsets[1] = {0};

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    if (vkBeginCommandBuffer(this->commandBufferList[x], &commandBufferBeginCreateInfo) == VK_SUCCESS) {
      printf("begin recording command buffer for image #%d\n", x);
    }

    vkCmdBeginRenderPass(this->commandBufferList[x], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(this->commandBufferList[x], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    vkCmdBindVertexBuffers(this->commandBufferList[x], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(this->commandBufferList[x], this->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    for (int y = 0; y < descriptorSetList.size(); y++) {
      vkCmdBindDescriptorSets(this->commandBufferList[x], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, y, 1, &descriptorSetList[y], 0, 0);
    }

    vkCmdDrawIndexed(this->commandBufferList[x], model->getPrimitiveCount() * 3, 1, 0, 0, 0);
    vkCmdEndRenderPass(this->commandBufferList[x]);

    { 
      VkImageMemoryBarrier imageMemoryBarrier = {};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.pNext = NULL;
      imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      imageMemoryBarrier.image = this->swapchainImageList[x];
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = 0;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

      vkCmdPipelineBarrier(this->commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    }

    { 
      VkImageMemoryBarrier imageMemoryBarrier = {};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.pNext = NULL;
      imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
      imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imageMemoryBarrier.image = this->rayTraceImage;
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = 0;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      vkCmdPipelineBarrier(this->commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    }

    {
      VkImageSubresourceLayers subresourceLayers = {};
      subresourceLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      subresourceLayers.mipLevel = 0;
      subresourceLayers.baseArrayLayer = 0;
      subresourceLayers.layerCount = 1;

      VkOffset3D offset = {};
      offset.x = 0;
      offset.y = 0;
      offset.z = 0;

      VkExtent3D extent = {};
      extent.width = 800;
      extent.height = 600;
      extent.depth = 1;

      VkImageCopy imageCopy = {};
      imageCopy.srcSubresource = subresourceLayers;
      imageCopy.srcOffset = offset;
      imageCopy.dstSubresource = subresourceLayers;
      imageCopy.dstOffset = offset;
      imageCopy.extent = extent;
  
      vkCmdCopyImage(this->commandBufferList[x], this->swapchainImageList[x], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, this->rayTraceImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
    }

    { 
      VkImageSubresourceRange subresourceRange = {};
      subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      subresourceRange.baseMipLevel = 0;
      subresourceRange.levelCount = 1;
      subresourceRange.baseArrayLayer = 0;
      subresourceRange.layerCount = 1;

      VkImageMemoryBarrier imageMemoryBarrier = {};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.pNext = NULL;
      imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      imageMemoryBarrier.image = this->swapchainImageList[x];
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      imageMemoryBarrier.dstAccessMask = 0;

      vkCmdPipelineBarrier(this->commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    }

    { 
      VkImageSubresourceRange subresourceRange = {};
      subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      subresourceRange.baseMipLevel = 0;
      subresourceRange.levelCount = 1;
      subresourceRange.baseArrayLayer = 0;
      subresourceRange.layerCount = 1;

      VkImageMemoryBarrier imageMemoryBarrier = {};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.pNext = NULL;
      imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
      imageMemoryBarrier.image = this->rayTraceImage;
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imageMemoryBarrier.dstAccessMask = 0;

      vkCmdPipelineBarrier(this->commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    }

    if (vkEndCommandBuffer(this->commandBufferList[x]) == VK_SUCCESS) {
      printf("end recording command buffer for image #%d\n", x);
    }
  }
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
        vkCreateFence(this->logicalDevice, &fenceCreateInfo, NULL, &this->inFlightFenceList[x]) == VK_SUCCESS) {
      printf("created synchronization objects for frame #%d\n", x);
    }
  }
}

void Device::updateCameraUniformBuffer(CameraUniform camera) {
  void* data;
  vkMapMemory(this->logicalDevice, this->cameraUniformBufferMemory, 0, sizeof(CameraUniform), 0, &data);
  memcpy(data, &camera, sizeof(CameraUniform));
  vkUnmapMemory(this->logicalDevice, this->cameraUniformBufferMemory);
}

void Device::drawFrame(CameraUniform camera) {
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
  submitInfo.pCommandBuffers = &this->commandBufferList[imageIndex];

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