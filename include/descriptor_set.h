#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>

#include "descriptor.h"

class DescriptorSet {
private:
  VkDescriptorSet descriptorSet;
  VkDescriptorSetLayout descriptorSetLayout;

  std::vector<Descriptor*> descriptorList;
public:
  DescriptorSet(std::vector<Descriptor*> descriptorList);
  ~DescriptorSet();

  VkDescriptorSet& getDescriptorSet();
  VkDescriptorSetLayout& getDescriptorSetLayout();

  std::vector<VkDescriptorSetLayoutBinding> getDescriptorSetLayoutBindingList();
  std::vector<VkWriteDescriptorSet> getWriteDescriptorSetList();
};