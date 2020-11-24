#pragma once

#include <vulkan/vulkan.h>

class DeviceDescriptor {
private:
  VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
  VkWriteDescriptorSet writeDescriptorSet;
public:
  DeviceDescriptor(uint32_t binding,
                   VkDescriptorType descriptorType, 
                   VkShaderStageFlagBits stageFlags,
                   VkDescriptorImageInfo* pImageInfo = NULL, 
                   VkDescriptorBufferInfo* pBufferInfo = NULL,
                   VkBufferView* pTexelBufferView = NULL,
                   void* pNext = NULL);

  ~DeviceDescriptor();

  VkDescriptorType getDescriptorType();
  VkDescriptorSetLayoutBinding getDescriptorSetLayoutBinding();
  VkWriteDescriptorSet getWriteDescriptorSet();

  void setDestinationDescriptorSet(VkDescriptorSet descriptorSet);
};