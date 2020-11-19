#include "device.h"

Device::Device(VkPhysicalDevice physicalDevice) {
  this->physicalDevice = physicalDevice;

  vkGetPhysicalDeviceProperties(physicalDevice, &this->physicalDeviceProperties);
  vkGetPhysicalDeviceFeatures(physicalDevice, &this->physicalDeviceFeatures);
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &this->physicalDeviceMemoryProperties);
}

Device::~Device() {

}

void Device::initializeDeviceQueue(VkSurfaceKHR surface) {
  this->deviceQueue = new DeviceQueue(this->physicalDevice, surface);
}

void Device::createLogicalDevice(std::vector<const char*> extensions) {
  float queuePriority = 1.0f;
  uint32_t deviceQueueCreateInfoCount = 3;

  std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoList(deviceQueueCreateInfoCount);

  deviceQueueCreateInfoList[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  deviceQueueCreateInfoList[0].pNext = NULL;
  deviceQueueCreateInfoList[0].flags = 0;
  deviceQueueCreateInfoList[0].queueFamilyIndex = this->deviceQueue->getGraphicsQueueIndex();
  deviceQueueCreateInfoList[0].queueCount = 1;
  deviceQueueCreateInfoList[0].pQueuePriorities = &queuePriority;
 
  deviceQueueCreateInfoList[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  deviceQueueCreateInfoList[1].pNext = NULL;
  deviceQueueCreateInfoList[1].flags = 0;
  deviceQueueCreateInfoList[1].queueFamilyIndex = this->deviceQueue->getPresentQueueIndex();
  deviceQueueCreateInfoList[1].queueCount = 1;
  deviceQueueCreateInfoList[1].pQueuePriorities = &queuePriority;

  deviceQueueCreateInfoList[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  deviceQueueCreateInfoList[2].pNext = NULL;
  deviceQueueCreateInfoList[2].flags = 0;
  deviceQueueCreateInfoList[2].queueFamilyIndex = this->deviceQueue->getComputeQueueIndex();
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

  this->deviceQueue->setQueueHandles(this->logicalDevice);
}