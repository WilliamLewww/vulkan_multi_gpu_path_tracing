#include "uniform_buffer_collection.h"

UniformBufferCollection::UniformBufferCollection(std::map<void*, uint32_t> bufferMap, VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties) {
  for (std::pair<void*, uint32_t> pair : bufferMap) {
    this->uniformBufferList.push_back(new UniformBuffer(logicalDevice, physicalDeviceMemoryProperties, pair.first, pair.second));
  }
}

UniformBufferCollection::~UniformBufferCollection() {
  for (int x = 0; x < this->uniformBufferList.size(); x++) {
    delete this->uniformBufferList[x];
  }
}

void UniformBufferCollection::updateUniformBuffer(VkDevice logicalDevice, int index, void* buffer, uint32_t bufferSize) {
  this->uniformBufferList[index]->updateUniformBuffer(logicalDevice, buffer, bufferSize);
}

VkDescriptorBufferInfo* UniformBufferCollection::getDescriptorBufferInfoPointer(int index) {
  return this->uniformBufferList[index]->getDescriptorBufferInfoPointer();
}