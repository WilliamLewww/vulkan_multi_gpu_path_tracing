#include "device_uniform_buffer_collection.h"

DeviceUniformBufferCollection::DeviceUniformBufferCollection(std::map<void*, uint32_t> bufferMap, VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties) {
  for (std::pair<void*, uint32_t> pair : bufferMap) {
    this->deviceUniformBufferList.push_back(new DeviceUniformBuffer(logicalDevice, physicalDeviceMemoryProperties, pair.first, pair.second));
  }
}

DeviceUniformBufferCollection::~DeviceUniformBufferCollection() {
  for (int x = 0; x < this->deviceUniformBufferList.size(); x++) {
    delete this->deviceUniformBufferList[x];
  }
}

void DeviceUniformBufferCollection::updateUniformBuffer(VkDevice logicalDevice, int index, void* buffer, uint32_t bufferSize) {
  this->deviceUniformBufferList[index]->updateUniformBuffer(logicalDevice, buffer, bufferSize);
}

VkDescriptorBufferInfo* DeviceUniformBufferCollection::getDescriptorBufferInfoPointer(int index) {
  return this->deviceUniformBufferList[index]->getDescriptorBufferInfoPointer();
}