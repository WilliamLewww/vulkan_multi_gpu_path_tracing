#include "storage_buffers.h"

StorageBuffers::StorageBuffers(VkDevice logicalDevice, 
               VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
               VkCommandPool commandPool,
               VkQueue queue) {

  VkDeviceSize bufferSize = sizeof(float) * 800 * 600 * 3;

  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties,
                              bufferSize, 
                              VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->rayDirectionBuffer, 
                              &this->rayDirectionBufferMemory);

  this->descriptorRayDirectionBufferInfo = {
    .buffer = this->rayDirectionBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
}

StorageBuffers::~StorageBuffers() {

}

VkDescriptorBufferInfo* StorageBuffers::getDescriptorRayDirectionBufferInfoPointer() {
  return &this->descriptorRayDirectionBufferInfo;
}