#include "descriptor_manager.h"

DescriptorManager::DescriptorManager() {

}

DescriptorManager::~DescriptorManager() {

}

void DescriptorManager::addDescriptorSet() {
  this->descriptorSetList.push_back(VkDescriptorSet());
  this->descriptorSetLayoutList.push_back(VkDescriptorSetLayout());
  this->descriptorSetLayoutBindingList.push_back(std::vector<VkDescriptorSetLayoutBinding>());
  this->writeDescriptorSetList.push_back(std::vector<VkWriteDescriptorSet>());
}

void DescriptorManager::addDescriptor(int descriptorSetIndex, 
                     uint32_t binding, 
                     VkDescriptorType descriptorType, 
                     VkShaderStageFlagBits stageFlags,
                     VkDescriptorImageInfo* pImageInfo, 
                     VkDescriptorBufferInfo* pBufferInfo,
                     VkBufferView* pTexelBufferView,
                     void* pNext) {

  VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {
    .binding = binding,
    .descriptorType = descriptorType,
    .descriptorCount = 1,
    .stageFlags = stageFlags,
    .pImmutableSamplers = NULL
  };
  this->descriptorSetLayoutBindingList[descriptorSetIndex].push_back(descriptorSetLayoutBinding);

  VkWriteDescriptorSet writeDescriptorSet = {
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .pNext = pNext,
    .dstSet = this->descriptorSetList[descriptorSetIndex],
    .dstBinding = binding,
    .dstArrayElement = 0,
    .descriptorCount = 1,
    .descriptorType = descriptorType,
    .pImageInfo = pImageInfo,
    .pBufferInfo = pBufferInfo,
    .pTexelBufferView = pTexelBufferView
  };
  this->writeDescriptorSetList[descriptorSetIndex].push_back(writeDescriptorSet);

  bool typeExists = false;
  for (int x = 0; x < this->descriptorPoolSizeList.size(); x++) {
    if (this->descriptorPoolSizeList[x].type == descriptorType) {
      this->descriptorPoolSizeList[x].descriptorCount += 1;
    }
  }

  if (!typeExists) {
    VkDescriptorPoolSize descriptorPoolSize = {
      .type = descriptorType,
      .descriptorCount = 1
    };
    this->descriptorPoolSizeList.push_back(descriptorPoolSize);
  }
}