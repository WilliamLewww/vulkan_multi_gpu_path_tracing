#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

class DescriptorManager {
private:
  std::vector<VkDescriptorSet> descriptorSetList;
  std::vector<VkDescriptorSetLayout> descriptorSetLayoutList;

  std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindingList;
  std::vector<std::vector<VkWriteDescriptorSet>> writeDescriptorSetList;

  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorPoolSize> descriptorPoolSizeList;
public:
  DescriptorManager(int descriptorSetCount);
  ~DescriptorManager();

  std::vector<VkDescriptorSetLayout> getDescriptorSetLayoutList();
  std::vector<VkDescriptorSet>& getDescriptorSetListReference();

  void addDescriptor(int descriptorSetIndex, 
                     uint32_t binding, 
                     VkDescriptorType descriptorType, 
                     VkShaderStageFlagBits stageFlags,
                     VkDescriptorImageInfo* pImageInfo = NULL, 
                     VkDescriptorBufferInfo* pBufferInfo = NULL,
                     VkBufferView* pTexelBufferView = NULL,
                     void* pNext = NULL);

  void concludeDescriptors(VkDevice logicalDevice);
};