#pragma once

#include <vulkan/vulkan.h>

class Descriptor {
private:
  VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
  VkWriteDescriptorSet writeDescriptorSet;
public:
  Descriptor(uint32_t binding,
             VkDescriptorType descriptorType, 
             uint32_t descriptorCount,
             VkShaderStageFlagBits stageFlags,
             VkDescriptorImageInfo* pImageInfo = NULL, 
             VkDescriptorBufferInfo* pBufferInfo = NULL,
             VkBufferView* pTexelBufferView = NULL,
             void* pNext = NULL);

  ~Descriptor();

  VkDescriptorType getDescriptorType();
  VkDescriptorSetLayoutBinding getDescriptorSetLayoutBinding();
  VkWriteDescriptorSet getWriteDescriptorSet();

  void setDestinationDescriptorSet(VkDescriptorSet descriptorSet);
};