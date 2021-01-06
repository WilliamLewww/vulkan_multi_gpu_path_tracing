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

VkPhysicalDevice Device::getPhysicalDevice() {
  return this->physicalDevice;
}

VkDevice Device::getLogicalDevice() {
  return this->logicalDevice;
}

uint32_t Device::getGraphicsQueueIndex() {
  return this->deviceQueue->getGraphicsQueueIndex();
}

VkQueue Device::getGraphicsQueue() {
  return this->deviceQueue->getGraphicsQueue();
}

uint32_t Device::getMinImageCount() {
  return 2;
}

uint32_t Device::getImageCount() {
  return this->swapchain->getSwapchainImageCount();
}

VkRenderPass Device::getRenderPass() {
  return this->renderPass->getRenderPass();
}

VkCommandPool Device::getCommandPool() {
  return this->commandPool->getCommandPool();
}

UniformBufferCollection* Device::getUniformBufferCollection() {
  return this->uniformBufferCollection;
}

AccelerationStructureSet* Device::getAccelerationStructureSet(int index) {
  return this->accelerationStructureCollection->getAccelerationStructureSet(index);
}

Textures* Device::getTextures() {
  return this->textures;
}

StorageBuffers* Device::getStorageBuffers() {
  return this->storageBuffers;
}

ModelInstanceSetCollection* Device::getModelInstanceSetCollection() {
  return this->modelInstanceSetCollection;
}

ModelInstanceSet* Device::getModelInstanceSet(int index) {
  return this->modelInstanceSetCollection->getModelInstanceSet(index);
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
  this->commandPool = new CommandPool(this->logicalDevice, this->deviceQueue->getGraphicsQueueIndex());
}

void Device::createSwapchain(VkSurfaceKHR surface) {
  this->swapchain = new Swapchain(this->logicalDevice, this->physicalDevice, surface, this->deviceQueue->getGraphicsQueueIndex(), this->deviceQueue->getPresentQueueIndex());
}

void Device::createRenderPass() {
  this->renderPass = new RenderPass(this->logicalDevice, this->swapchain->getSwapchainImageFormat());
}

void Device::createTextures() {
  this->textures = new Textures(this->logicalDevice, 
                                            this->physicalDeviceMemoryProperties, 
                                            this->swapchain->getSwapchainImageFormat(),
                                            this->swapchain->getSwapchainExtent(),
                                            this->commandPool->getCommandPool(), 
                                            this->deviceQueue->getGraphicsQueue());
}

void Device::createStorageBuffers() {
  this->storageBuffers = new StorageBuffers(this->logicalDevice, 
                                            this->physicalDeviceMemoryProperties, 
                                            this->commandPool->getCommandPool(),
                                            this->deviceQueue->getGraphicsQueue());
}

void Device::createFramebuffers() {
  this->framebuffers = new Framebuffers(this->logicalDevice, 
                                                    this->swapchain->getSwapchainImageCount(), 
                                                    this->swapchain->getSwapchainExtent(),
                                                    this->swapchain->getSwapchainImageViewList(),
                                                    this->renderPass->getRenderPass(),
                                                    this->textures->getDepthImageView());
}

void Device::createModelInstanceCollection(std::vector<std::map<Model*, std::vector<TRS>>> modelFrequencyMapList) {
  this->modelInstanceSetCollection = new ModelInstanceSetCollection(modelFrequencyMapList,
                                                              this->logicalDevice, 
                                                              this->physicalDeviceMemoryProperties, 
                                                              this->commandPool->getCommandPool(),
                                                              this->deviceQueue->getGraphicsQueue());
}

void Device::createUniformBufferCollection(std::map<void*, uint32_t> bufferMap) {
  this->uniformBufferCollection = new UniformBufferCollection(bufferMap, this->logicalDevice, this->physicalDeviceMemoryProperties);
}

void Device::createAccelerationStructureCollection() {
  this->accelerationStructureCollection = new AccelerationStructureCollection(this->modelInstanceSetCollection->getModelInstanceMapList(), this->logicalDevice, this->physicalDeviceMemoryProperties, this->commandPool->getCommandPool(), this->deviceQueue->getComputeQueue());
}

void Device::createDescriptorSetCollection(std::vector<std::vector<Descriptor*>> separatedDescriptorList) {
  this->descriptorSetCollection = new DescriptorSetCollection(separatedDescriptorList, this->logicalDevice);
}

void Device::createGraphicsPipelineCollection(std::vector<std::vector<std::string>> shaderList) {
  this->graphicsPipelineCollection = new GraphicsPipelineCollection(shaderList, this->descriptorSetCollection->getDescriptorSetLayoutList(), this->logicalDevice, this->swapchain->getSwapchainExtent(), this->renderPass->getRenderPass());
}

void Device::createRenderCommandBuffers() {
  this->renderCommandBuffers = new RenderCommandBuffers(this->logicalDevice, 
                                                        this->swapchain->getSwapchainImageCount(), 
                                                        this->commandPool->getCommandPool());
}

void Device::createSynchronizationObjects() {
  this->synchronizationObjects = new SynchronizationObjects(this->logicalDevice, this->framesInFlight, this->swapchain->getSwapchainImageCount());
}

void Device::updateAccelerationStructureSet(int index) {
  this->accelerationStructureCollection->getAccelerationStructureSet(index)->updateAccelerationStructure(this->modelInstanceSetCollection->getModelInstanceSet(0)->getModelInstanceList(), this->logicalDevice, this->physicalDeviceMemoryProperties, this->commandPool->getCommandPool(), this->deviceQueue->getComputeQueue());
}

void Device::drawFrame(int index) {
  vkWaitForFences(this->logicalDevice, 1, &this->synchronizationObjects->getInFlightFence(this->currentFrame), VK_TRUE, UINT64_MAX);
    
  uint32_t imageIndex;
  vkAcquireNextImageKHR(this->logicalDevice, this->swapchain->getSwapchain(), UINT64_MAX, this->synchronizationObjects->getImageAvailableSemaphore(this->currentFrame), VK_NULL_HANDLE, &imageIndex);
    
  if (this->synchronizationObjects->getImageInFlight(imageIndex) != VK_NULL_HANDLE) {
    vkWaitForFences(this->logicalDevice, 1, &this->synchronizationObjects->getImageInFlight(imageIndex), VK_TRUE, UINT64_MAX);
  }
  this->synchronizationObjects->getImageInFlight(imageIndex) = this->synchronizationObjects->getInFlightFence(this->currentFrame);

  VkSemaphore waitSemaphores[1] = {this->synchronizationObjects->getImageAvailableSemaphore(this->currentFrame)};
  VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signalSemaphores[1] = {this->synchronizationObjects->getRenderFinishedSemaphore(this->currentFrame)};
   
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

  vkResetFences(this->logicalDevice, 1, &this->synchronizationObjects->getInFlightFence(this->currentFrame));

  this->renderCommandBuffers->recreateCommandBuffer(imageIndex,
                                                    this->swapchain->getSwapchainExtent(), 
                                                    this->renderPass->getRenderPass(), 
                                                    this->framebuffers->getFramebufferList(),
                                                    this->swapchain->getSwapchainImageList(),
                                                    this->textures->getRayTraceImage(),
                                                    this->graphicsPipelineCollection->getGraphicsPipelineList(),
                                                    this->graphicsPipelineCollection->getPipelineLayoutList(),
                                                    this->descriptorSetCollection->getDescriptorSetList(),
                                                    this->modelInstanceSetCollection->getModelInstanceSet(index)->getModelInstanceList(),
                                                    this->modelInstanceSetCollection->getCollectionOffset(index),
                                                    true);

  if (vkQueueSubmit(this->deviceQueue->getGraphicsQueue(), 1, &submitInfo, this->synchronizationObjects->getInFlightFence(this->currentFrame)) != VK_SUCCESS) {
    printf("failed to submit draw command buffer\n");
  }

  VkSwapchainKHR swapchains[1] = {this->swapchain->getSwapchain()};

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
  this->uniformBufferCollection->updateUniformBuffer(this->logicalDevice, index, buffer, bufferSize);
}