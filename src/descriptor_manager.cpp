#include "descriptor_manager.h"

DescriptorManager::DescriptorManager() {

}

DescriptorManager::~DescriptorManager() {

}

void DescriptorManager::addDescriptorSet() {
  this->descriptorSetList.push_back(VkDescriptorSet());
  this->descriptorSetLayoutList.push_back(VkDescriptorSetLayout());
  this->descriptorSetLayoutBindingList.push_back(std::vector<VkDescriptorSetLayoutBinding>());
}

void DescriptorManager::addDescriptorSetLayoutBinding(int descriptorSetIndex, uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlagBits stageFlags) {
  VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {
    .binding = binding,
    .descriptorType = descriptorType,
    .descriptorCount = 1,
    .stageFlags = stageFlags,
    .pImmutableSamplers = NULL
  };
  descriptorSetLayoutBindingList[descriptorSetIndex].push_back(descriptorSetLayoutBinding);

  bool typeExists = false;
  for (int x = 0; x < descriptorPoolSizeList.size(); x++) {
    if (descriptorPoolSizeList[x].type == descriptorType) {
      descriptorPoolSizeList[x].descriptorCount += 1;
    }
  }

  if (!typeExists) {
    VkDescriptorPoolSize descriptorPoolSize = {
      .type = descriptorType,
      .descriptorCount = 1
    };
    descriptorPoolSizeList.push_back(descriptorPoolSize);
  }
}