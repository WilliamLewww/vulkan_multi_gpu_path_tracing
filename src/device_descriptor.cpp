#include "device_descriptor.h"

DeviceDescriptor::DeviceDescriptor(uint32_t binding,
                                   VkDescriptorType descriptorType, 
                                   VkShaderStageFlagBits stageFlags,
                                   VkDescriptorImageInfo* pImageInfo, 
                                   VkDescriptorBufferInfo* pBufferInfo,
                                   VkBufferView* pTexelBufferView,
                                   void* pNext) {

  this->descriptorSetLayoutBinding = {
    .binding = binding,
    .descriptorType = descriptorType,
    .descriptorCount = 1,
    .stageFlags = stageFlags,
    .pImmutableSamplers = NULL
  };

  this->writeDescriptorSet = {
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .pNext = pNext,
    .dstSet = VK_NULL_HANDLE,
    .dstBinding = binding,
    .dstArrayElement = 0,
    .descriptorCount = 1,
    .descriptorType = descriptorType,
    .pImageInfo = pImageInfo,
    .pBufferInfo = pBufferInfo,
    .pTexelBufferView = pTexelBufferView
  };
}

DeviceDescriptor::~DeviceDescriptor() {

}

VkDescriptorType DeviceDescriptor::getDescriptorType() {
  return this->descriptorSetLayoutBinding.descriptorType;
}

void DeviceDescriptor::setDestinationDescriptorSet(VkDescriptorSet descriptorSet) {
  this->writeDescriptorSet.dstSet = descriptorSet;
}

VkDescriptorSetLayoutBinding DeviceDescriptor::getDescriptorSetLayoutBinding() {
  return this->descriptorSetLayoutBinding;
}

VkWriteDescriptorSet DeviceDescriptor::getWriteDescriptorSet() {
  return this->writeDescriptorSet;
}