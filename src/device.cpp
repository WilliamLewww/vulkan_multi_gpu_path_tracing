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