#include "device.h"

Device::Device(VkPhysicalDevice physicalDevice) {
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

  float queuePriority = 1.0f;
  uint32_t deviceQueueCreateInfoCount = queueFrequencyMap.size();

  std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoList;

  for (std::pair<uint32_t, uint32_t> pair : queueFrequencyMap) {
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .queueFamilyIndex = pair.first,
      .queueCount = pair.second,
      .pQueuePriorities = &queuePriority
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

  VkPhysicalDeviceRayTracingFeaturesKHR rayTracingFeatures = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_FEATURES_KHR,
    .pNext = &bufferDeviceAddressFeatures,
    .rayTracing = VK_TRUE,
    .rayTracingShaderGroupHandleCaptureReplay = VK_FALSE,
    .rayTracingShaderGroupHandleCaptureReplayMixed = VK_FALSE,
    .rayTracingAccelerationStructureCaptureReplay = VK_FALSE,
    .rayTracingIndirectTraceRays = VK_FALSE,
    .rayTracingIndirectAccelerationStructureBuild = VK_FALSE,
    .rayTracingHostAccelerationStructureCommands = VK_FALSE,
    .rayQuery = VK_TRUE,
    .rayTracingPrimitiveCulling = VK_FALSE
  };

  // incomplete listing due to large amount of data members
  VkPhysicalDeviceFeatures deviceFeatures = {
    .geometryShader = VK_TRUE,
    .fragmentStoresAndAtomics = VK_TRUE
  };

  VkDeviceCreateInfo deviceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &rayTracingFeatures,
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

void Device::createModelInstances(std::map<Model*, uint32_t> modelFrequencyMap) {
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
                                                        this->graphicsPipeline->getGraphicsPipeline(),
                                                        this->graphicsPipeline->getPipelineLayout(),
                                                        this->deviceDescriptorSetCollection->getDescriptorSetList(),
                                                        this->modelInstanceCollection->getModelInstanceList());
}

void Device::createSynchronizationObjects() {
  this->imageAvailableSemaphoreList.resize(2);
  this->renderFinishedSemaphoreList.resize(2);
  this->inFlightFenceList.resize(2);
  this->imageInFlightList.resize(this->deviceSwapchain->getSwapchainImageCount());
  for (int x = 0; x < this->deviceSwapchain->getSwapchainImageCount(); x++) {
    this->imageInFlightList[x] = VK_NULL_HANDLE;
  }

  VkSemaphoreCreateInfo semaphoreCreateInfo = {};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceCreateInfo = {};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (int x = 0; x < 2; x++) {
    if (vkCreateSemaphore(this->logicalDevice, &semaphoreCreateInfo, NULL, &this->imageAvailableSemaphoreList[x]) == VK_SUCCESS &&
        vkCreateSemaphore(this->logicalDevice, &semaphoreCreateInfo, NULL, &this->renderFinishedSemaphoreList[x]) == VK_SUCCESS &&
        vkCreateFence(this->logicalDevice, &fenceCreateInfo, NULL, &this->inFlightFenceList[x]) != VK_SUCCESS) {
      printf("failed to create synchronization objects for frame #%d\n", x);
    }
  }
}

void Device::updateCameraUniformBuffer(VkDeviceMemory uniformBufferMemory, void* buffer, uint32_t bufferSize) {
  void* data;
  vkMapMemory(this->logicalDevice, uniformBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, buffer, bufferSize);
  vkUnmapMemory(this->logicalDevice, uniformBufferMemory);
}

void Device::drawFrame(void* buffer, uint32_t bufferSize) {
  vkWaitForFences(this->logicalDevice, 1, &this->inFlightFenceList[this->currentFrame], VK_TRUE, UINT64_MAX);
    
  uint32_t imageIndex;
  vkAcquireNextImageKHR(this->logicalDevice, this->deviceSwapchain->getSwapchain(), UINT64_MAX, this->imageAvailableSemaphoreList[this->currentFrame], VK_NULL_HANDLE, &imageIndex);
    
  if (this->imageInFlightList[imageIndex] != VK_NULL_HANDLE) {
    vkWaitForFences(this->logicalDevice, 1, &this->imageInFlightList[imageIndex], VK_TRUE, UINT64_MAX);
  }
  this->imageInFlightList[imageIndex] = this->inFlightFenceList[this->currentFrame];
 
  updateCameraUniformBuffer(this->deviceUniformBufferCollection->getDeviceMemory(0), buffer, bufferSize);
   
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
  VkSemaphore waitSemaphores[1] = {this->imageAvailableSemaphoreList[this->currentFrame]};
  VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &this->renderCommandBuffers->getCommandBufferList()[imageIndex];

  VkSemaphore signalSemaphores[1] = {this->renderFinishedSemaphoreList[this->currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  vkResetFences(this->logicalDevice, 1, &this->inFlightFenceList[this->currentFrame]);

  if (vkQueueSubmit(this->deviceQueue->getGraphicsQueue(), 1, &submitInfo, this->inFlightFenceList[this->currentFrame]) != VK_SUCCESS) {
    printf("failed to submit draw command buffer\n");
  }

  VkPresentInfoKHR presentInfo = {};  
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapchains[1] = {this->deviceSwapchain->getSwapchain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(this->deviceQueue->getPresentQueue(), &presentInfo);

  this->currentFrame = (this->currentFrame + 1) % 2;
}