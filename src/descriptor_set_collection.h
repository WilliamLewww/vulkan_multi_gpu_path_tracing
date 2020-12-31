#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

#include "descriptor_set.h"

class DescriptorSetCollection {
private:
  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorPoolSize> descriptorPoolSizeList;

  std::vector<DescriptorSet*> descriptorSetList;
public:
  DescriptorSetCollection(std::vector<std::vector<Descriptor*>> separatedDescriptorList, VkDevice logicalDevice);
  ~DescriptorSetCollection();

  std::vector<VkDescriptorSetLayout> getDescriptorSetLayoutList();
  std::vector<VkDescriptorSet> getDescriptorSetList();
};