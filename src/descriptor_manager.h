#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class DescriptorManager {
private:
  std::vector<VkDescriptorSet> descriptorSetList;
  std::vector<VkDescriptorSetLayout> descriptorSetLayoutList;
  std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindingList;

  std::vector<VkDescriptorPoolSize> descriptorPoolSizeList;
public:
  DescriptorManager();
  ~DescriptorManager();

  void addDescriptorSet();
  void addDescriptorSetLayoutBinding(int descriptorSetIndex, uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlagBits stageFlags);
};