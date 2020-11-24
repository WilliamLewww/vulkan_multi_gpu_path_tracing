#include "device_uniform_buffer.h"

DeviceUniformBuffer::DeviceUniformBuffer(VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, void* buffer, uint32_t bufferSize) {
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              bufferSize, 
                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &this->uniformBuffer, 
                              &this->uniformBufferMemory);

  void* data;
  vkMapMemory(logicalDevice, this->uniformBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, buffer, bufferSize);
  vkUnmapMemory(logicalDevice, this->uniformBufferMemory);

  this->descriptorBufferInfo = {
    .buffer= this->uniformBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
}

DeviceUniformBuffer::~DeviceUniformBuffer() {

}

VkDeviceMemory DeviceUniformBuffer::getDeviceMemory() {
  return this->uniformBufferMemory;
}

VkDescriptorBufferInfo* DeviceUniformBuffer::getDescriptorBufferInfoPointer() {
  return &this->descriptorBufferInfo;
}