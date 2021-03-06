#include "descriptor.h"

Descriptor::Descriptor(uint32_t binding,
                       VkDescriptorType descriptorType,
                       uint32_t descriptorCount,
                       VkShaderStageFlagBits stageFlags,
                       VkDescriptorImageInfo* pImageInfo, 
                       VkDescriptorBufferInfo* pBufferInfo,
                       VkBufferView* pTexelBufferView,
                       void* pNext) {

  this->descriptorSetLayoutBinding = {
    .binding = binding,
    .descriptorType = descriptorType,
    .descriptorCount = descriptorCount,
    .stageFlags = stageFlags,
    .pImmutableSamplers = NULL
  };

  this->writeDescriptorSet = {
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .pNext = pNext,
    .dstSet = VK_NULL_HANDLE,
    .dstBinding = binding,
    .dstArrayElement = 0,
    .descriptorCount = descriptorCount,
    .descriptorType = descriptorType,
    .pImageInfo = pImageInfo,
    .pBufferInfo = pBufferInfo,
    .pTexelBufferView = pTexelBufferView
  };
}

Descriptor::~Descriptor() {

}

VkDescriptorType Descriptor::getDescriptorType() {
  return this->descriptorSetLayoutBinding.descriptorType;
}

void Descriptor::setDestinationDescriptorSet(VkDescriptorSet descriptorSet) {
  this->writeDescriptorSet.dstSet = descriptorSet;
}

VkDescriptorSetLayoutBinding Descriptor::getDescriptorSetLayoutBinding() {
  return this->descriptorSetLayoutBinding;
}

VkWriteDescriptorSet Descriptor::getWriteDescriptorSet() {
  return this->writeDescriptorSet;
}