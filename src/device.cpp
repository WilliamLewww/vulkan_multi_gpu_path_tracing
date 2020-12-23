#include "device.h"

Device::Device(VkPhysicalDevice physicalDevice) {
  this->currentFrame = 0;
  this->framesInFlight = 2;

  this->physicalDevice = physicalDevice;

  vkGetPhysicalDeviceProperties(physicalDevice, &this->physicalDeviceProperties);
  vkGetPhysicalDeviceFeatures(physicalDevice, &this->physicalDeviceFeatures);
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &this->physicalDeviceMemoryProperties);
}

Device::~Device() {

}

ModelInstanceCollection* Device::getModelInstanceCollectionPointer() {
  return this->modelInstanceCollection;
}

DeviceUniformBufferCollection* Device::getDeviceUniformBufferCollection() {
  return this->deviceUniformBufferCollection;
}

AccelerationStructureCollection* Device::getAccelerationStructureCollection() {
  return this->accelerationStructureCollection;
}

DeviceTextures* Device::getDeviceTextures() {
  return this->deviceTextures;
}

ModelInstanceCollection* Device::getModelInstanceCollection() {
  return this->modelInstanceCollection;
}

void Device::initializeDeviceQueue(VkSurfaceKHR surface) {
  this->deviceQueue = new DeviceQueue(this->physicalDevice, surface);
}

void Device::createLogicalDevice(std::vector<const char*> extensions) {
  std::map<uint32_t, uint32_t> queueFrequencyMap = this->deviceQueue->getQueueFrequencyMap();

  std::vector<float> queuePriorityList = {1.0f, 1.0f};
  uint32_t deviceQueueCreateInfoCount = queueFrequencyMap.size();

  std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoList;

  for (std::pair<uint32_t, uint32_t> pair : queueFrequencyMap) {
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .queueFamilyIndex = pair.first,
      .queueCount = pair.second,
      .pQueuePriorities = queuePriorityList.data()
    };

    deviceQueueCreateInfoList.push_back(deviceQueueCreateInfo);
  }

  VkPhysicalDeviceBufferDeviceAddressFeaturesEXT bufferDeviceAddressFeatures = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT,
    .pNext = NULL,
    .bufferDeviceAddress = VK_TRUE,
    .bufferDeviceAddressCaptureReplay = VK_FALSE,
    .bufferDeviceAddressMultiDevice = VK_FALSE
  };

  VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
    .pNext = &bufferDeviceAddressFeatures,
    .rayQuery = VK_TRUE
  };

  VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
    .pNext = &rayQueryFeatures,
    .accelerationStructure = VK_TRUE,
    .accelerationStructureCaptureReplay = VK_TRUE,
    .accelerationStructureIndirectBuild = VK_FALSE,
    .accelerationStructureHostCommands = VK_FALSE,
    .descriptorBindingAccelerationStructureUpdateAfterBind = VK_FALSE
  };

  // incomplete listing due to large amount of data members
  VkPhysicalDeviceFeatures deviceFeatures = {
    .geometryShader = VK_TRUE,
    .fragmentStoresAndAtomics = VK_TRUE
  };

  VkDeviceCreateInfo deviceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &accelerationStructureFeatures,
    .flags = 0,
    .queueCreateInfoCount = deviceQueueCreateInfoCount,
    .pQueueCreateInfos = deviceQueueCreateInfoList.data(),
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = NULL,
    .enabledExtensionCount = (uint32_t)extensions.size(),
    .ppEnabledExtensionNames = extensions.data(),
    .pEnabledFeatures = &deviceFeatures
  };

  if (vkCreateDevice(this->physicalDevice, &deviceCreateInfo, NULL, &this->logicalDevice) != VK_SUCCESS) {
    printf("failed to created logical connection to device\n");
  }

  this->deviceQueue->setQueueHandles(this->logicalDevice);
}

void Device::createCommandPool() {
  this->deviceCommandPool = new DeviceCommandPool(this->logicalDevice, this->deviceQueue->getGraphicsQueueIndex());
}

void Device::createSwapchain(VkSurfaceKHR surface) {
  this->deviceSwapchain = new DeviceSwapchain(this->logicalDevice, this->physicalDevice, surface, this->deviceQueue->getGraphicsQueueIndex(), this->deviceQueue->getPresentQueueIndex());
}

void Device::createRenderPass() {
  this->deviceRenderPass = new DeviceRenderPass(this->logicalDevice, this->deviceSwapchain->getSwapchainImageFormat());
}

void Device::createDepthResource() {
  this->deviceDepthResource = new DeviceDepthResource(this->logicalDevice, this->physicalDeviceMemoryProperties, this->deviceSwapchain->getSwapchainExtent());
}

void Device::createFramebuffers() {
  this->deviceFramebuffers = new DeviceFramebuffers(this->logicalDevice, 
                                                    this->deviceSwapchain->getSwapchainImageCount(), 
                                                    this->deviceSwapchain->getSwapchainExtent(),
                                                    this->deviceSwapchain->getSwapchainImageViewList(),
                                                    this->deviceRenderPass->getRenderPass(),
                                                    this->deviceDepthResource->getDepthImageView());
}

void Device::createTextures() {
  this->deviceTextures = new DeviceTextures(this->logicalDevice, 
                                            this->physicalDeviceMemoryProperties, 
                                            this->deviceSwapchain->getSwapchainImageFormat(), 
                                            this->deviceCommandPool->getCommandPool(), 
                                            this->deviceQueue->getGraphicsQueue());
}

void Device::createModelInstances(std::map<Model*, std::vector<Matrix4x4>> modelFrequencyMap) {
  this->modelInstanceCollection = new ModelInstanceCollection(modelFrequencyMap,
                                                              this->logicalDevice, 
                                                              this->physicalDeviceMemoryProperties, 
                                                              this->deviceCommandPool->getCommandPool(),
                                                              this->deviceQueue->getGraphicsQueue());
}

void Device::createUniformBufferCollection(std::map<void*, uint32_t> bufferMap) {
  this->deviceUniformBufferCollection = new DeviceUniformBufferCollection(bufferMap, this->logicalDevice, this->physicalDeviceMemoryProperties);
}

void Device::createAccelerationStructureCollection(std::map<Model*, std::vector<ModelInstance*>> modelInstanceMap) {
  this->accelerationStructureCollection = new AccelerationStructureCollection(modelInstanceMap, this->logicalDevice, this->physicalDeviceMemoryProperties, this->deviceCommandPool->getCommandPool(), this->deviceQueue->getComputeQueue());
}

void Device::createDescriptorSetCollection(std::vector<std::vector<DeviceDescriptor*>> separatedDeviceDescriptorList) {
  this->deviceDescriptorSetCollection = new DeviceDescriptorSetCollection(separatedDeviceDescriptorList, this->logicalDevice);
}

void Device::createGraphicsPipeline(std::string vertexShaderFile, std::string fragmentShaderFile) {
  this->depthGraphicsPipeline = new GraphicsPipeline(vertexShaderFile, this->deviceDescriptorSetCollection->getDescriptorSetLayoutList(), this->logicalDevice, this->deviceSwapchain->getSwapchainExtent(), this->deviceRenderPass->getRenderPass()); 
  this->graphicsPipeline = new GraphicsPipeline(vertexShaderFile, fragmentShaderFile, this->deviceDescriptorSetCollection->getDescriptorSetLayoutList(), this->logicalDevice, this->deviceSwapchain->getSwapchainExtent(), this->deviceRenderPass->getRenderPass());
}

void Device::createRenderCommandBuffers() {
  this->renderCommandBuffers = new RenderCommandBuffers(this->logicalDevice, 
                                                        this->deviceSwapchain->getSwapchainImageCount(), 
                                                        this->deviceSwapchain->getSwapchainExtent(), 
                                                        this->deviceCommandPool->getCommandPool(), 
                                                        this->deviceRenderPass->getRenderPass(), 
                                                        this->deviceFramebuffers->getFramebufferList(),
                                                        this->deviceSwapchain->getSwapchainImageList(),
                                                        this->deviceTextures->getRayTraceImage(),
                                                        this->depthGraphicsPipeline->getGraphicsPipeline(),
                                                        this->graphicsPipeline->getGraphicsPipeline(),
                                                        this->graphicsPipeline->getPipelineLayout(),
                                                        this->deviceDescriptorSetCollection->getDescriptorSetList(),
                                                        this->modelInstanceCollection->getModelInstanceList());
}

void Device::createSynchronizationObjects() {
  this->synchronizationObjects = new SynchronizationObjects(this->logicalDevice, this->framesInFlight, this->deviceSwapchain->getSwapchainImageCount());
}

void Device::drawFrame() {
  VkSemaphore imageAvailableSemaphore = synchronizationObjects->getImageAvailableSemaphore(this->currentFrame);
  VkSemaphore renderFinishedSemaphore = synchronizationObjects->getRenderFinishedSemaphore(this->currentFrame);
  VkFence inFlightFence = synchronizationObjects->getInFlightFence(this->currentFrame);
  VkFence imageInFlight = synchronizationObjects->getImageInFlight(this->currentFrame);

  vkWaitForFences(this->logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    
  uint32_t imageIndex;
  vkAcquireNextImageKHR(this->logicalDevice, this->deviceSwapchain->getSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    
  if (imageInFlight != VK_NULL_HANDLE) {
    vkWaitForFences(this->logicalDevice, 1, &imageInFlight, VK_TRUE, UINT64_MAX);
  }
  imageInFlight = inFlightFence;

  VkSemaphore waitSemaphores[1] = {imageAvailableSemaphore};
  VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signalSemaphores[1] = {renderFinishedSemaphore};
   
  VkSubmitInfo submitInfo = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = NULL,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = waitSemaphores,
    .pWaitDstStageMask = waitStages,
    .commandBufferCount = 1,
    .pCommandBuffers = &this->renderCommandBuffers->getCommandBufferList()[imageIndex],
    .signalSemaphoreCount = 1,
    .pSignalSemaphores = signalSemaphores,
  };

  vkResetFences(this->logicalDevice, 1, &inFlightFence);

  if (vkQueueSubmit(this->deviceQueue->getGraphicsQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
    printf("failed to submit draw command buffer\n");
  }

  VkSwapchainKHR swapchains[1] = {this->deviceSwapchain->getSwapchain()};

  VkPresentInfoKHR presentInfo = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pNext = NULL,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = signalSemaphores,
    .swapchainCount = 1,
    .pSwapchains = swapchains,
    .pImageIndices = &imageIndex,
    .pResults = NULL,
  };

  vkQueuePresentKHR(this->deviceQueue->getPresentQueue(), &presentInfo);

  this->currentFrame = (this->currentFrame + 1) % this->framesInFlight;
}

void Device::updateUniformBuffer(int index, void* buffer, uint32_t bufferSize) {
  this->deviceUniformBufferCollection->updateUniformBuffer(this->logicalDevice, index, buffer, bufferSize);
}