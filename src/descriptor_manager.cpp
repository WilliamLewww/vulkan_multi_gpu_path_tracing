#include "descriptor_manager.h"

DescriptorManager::DescriptorManager(int descriptorSetCount) {
  for (int x = 0; x < descriptorSetCount; x++) {
    this->descriptorSetList.push_back(VkDescriptorSet());
    this->descriptorSetLayoutList.push_back(VkDescriptorSetLayout());
    this->descriptorSetLayoutBindingList.push_back(std::vector<VkDescriptorSetLayoutBinding>());
    this->writeDescriptorSetList.push_back(std::vector<VkWriteDescriptorSet>());
  }
}

DescriptorManager::~DescriptorManager() {

}

std::vector<VkDescriptorSetLayout> DescriptorManager::getDescriptorSetLayoutList() {
  return this->descriptorSetLayoutList;
}

std::vector<VkDescriptorSet>& DescriptorManager:: getDescriptorSetListReference() {
  return this->descriptorSetList;
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

void DescriptorManager::concludeDescriptors(VkDevice logicalDevice) {
  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
  descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolCreateInfo.poolSizeCount = this->descriptorPoolSizeList.size();
  descriptorPoolCreateInfo.pPoolSizes = this->descriptorPoolSizeList.data();
  descriptorPoolCreateInfo.maxSets = this->descriptorSetList.size();

  if (vkCreateDescriptorPool(logicalDevice, &descriptorPoolCreateInfo, NULL, &this->descriptorPool) == VK_SUCCESS) {
    printf("%s\n", "created descriptor pool");
  }

  for (int x = 0; x < this->descriptorSetList.size(); x++) {
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = this->descriptorSetLayoutBindingList[x].size();
    descriptorSetLayoutCreateInfo.pBindings = this->descriptorSetLayoutBindingList[x].data();
    
    if (vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCreateInfo, NULL, &this->descriptorSetLayoutList[x]) == VK_SUCCESS) {
      printf("%s\n", "created descriptor set layout");
    }

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = this->descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &this->descriptorSetLayoutList[x];

    if (vkAllocateDescriptorSets(logicalDevice, &descriptorSetAllocateInfo, &this->descriptorSetList[x]) == VK_SUCCESS) {
      printf("%s\n", "allocated descriptor sets");
    }

    for (int y = 0; y < this->writeDescriptorSetList[x].size(); y++) {
      this->writeDescriptorSetList[x][y].dstSet = this->descriptorSetList[x];
    }

    vkUpdateDescriptorSets(logicalDevice, this->descriptorSetLayoutBindingList[x].size(), this->writeDescriptorSetList[x].data(), 0, NULL);
  }
}