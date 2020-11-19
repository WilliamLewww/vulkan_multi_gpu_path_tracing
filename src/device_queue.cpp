#include "device_queue.h"

DeviceQueue::DeviceQueue(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

  this->queueFamilyPropertiesList.resize(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, this->queueFamilyPropertiesList.data());

  this->graphicsQueueIndex = -1;
  this->presentQueueIndex = -1;
  this->computeQueueIndex = -1;

  for (int x = 0; x < queueFamilyCount; x++) {
    if (this->graphicsQueueIndex == -1 && queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      this->graphicsQueueIndex = x;
    }

    if (this->computeQueueIndex == -1 && this->graphicsQueueIndex != x && queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_COMPUTE_BIT) {
      this->computeQueueIndex = x;
    }

    VkBool32 isPresentSupported = 0;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, x, surface, &isPresentSupported);
    
    if (this->presentQueueIndex == -1 && this->graphicsQueueIndex != x && this->computeQueueIndex != x && isPresentSupported) {
      this->presentQueueIndex = x;
    }
  
    if (this->graphicsQueueIndex != -1 && this->presentQueueIndex != -1 && this->computeQueueIndex != -1) {
      break;
    }
  }
}

DeviceQueue::~DeviceQueue() {

}

uint32_t DeviceQueue::getGraphicsQueueIndex() {
  return this->graphicsQueueIndex;
}

uint32_t DeviceQueue::getPresentQueueIndex() {
  return this->presentQueueIndex;
}

uint32_t DeviceQueue::getComputeQueueIndex() {
  return this->computeQueueIndex;
}

void DeviceQueue::setQueueHandles(VkDevice logicalDevice) {
  vkGetDeviceQueue(logicalDevice, this->graphicsQueueIndex, 0, &this->graphicsQueue);
  vkGetDeviceQueue(logicalDevice, this->presentQueueIndex, 0, &this->presentQueue);
  vkGetDeviceQueue(logicalDevice, this->computeQueueIndex, 0, &this->computeQueue);
}