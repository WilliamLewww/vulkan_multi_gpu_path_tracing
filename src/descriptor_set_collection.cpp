#include "descriptor_set_collection.h"

DescriptorSetCollection::DescriptorSetCollection(std::vector<std::vector<Descriptor*>> separatedDescriptorList, VkDevice logicalDevice) {
  for (int x = 0; x < separatedDescriptorList.size(); x++) {
    this->descriptorSetList.push_back(new DescriptorSet(separatedDescriptorList[x]));
    
    for (int y = 0; y < separatedDescriptorList[x].size(); y++) {
      bool typeExists = false;
      for (int z = 0; z < this->descriptorPoolSizeList.size(); z++) {
        if (this->descriptorPoolSizeList[z].type == separatedDescriptorList[x][y]->getDescriptorType()) {
          this->descriptorPoolSizeList[z].descriptorCount += 1;
          typeExists = true;
        }
      }

      if (!typeExists) {
        VkDescriptorPoolSize descriptorPoolSize = {
          .type = separatedDescriptorList[x][y]->getDescriptorType(),
          .descriptorCount = 1
        };
        this->descriptorPoolSizeList.push_back(descriptorPoolSize);
      }
    }
  }

  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .maxSets = (uint32_t)this->descriptorSetList.size(),
    .poolSizeCount = (uint32_t)this->descriptorPoolSizeList.size(),
    .pPoolSizes = this->descriptorPoolSizeList.data()
  };

  if (vkCreateDescriptorPool(logicalDevice, &descriptorPoolCreateInfo, NULL, &this->descriptorPool) != VK_SUCCESS) {
    printf("%s\n", "failed to create descriptor pool");
  }

  for (int x = 0; x < this->descriptorSetList.size(); x++) {
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindingList = this->descriptorSetList[x]->getDescriptorSetLayoutBindingList();
    std::vector<VkWriteDescriptorSet> writeDescriptorSetList = this->descriptorSetList[x]->getWriteDescriptorSetList();

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .bindingCount = (uint32_t)descriptorSetLayoutBindingList.size(),
      .pBindings = descriptorSetLayoutBindingList.data(),
    };
    
    if (vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCreateInfo, NULL, &this->descriptorSetList[x]->getDescriptorSetLayout()) != VK_SUCCESS) {
      printf("%s\n", "failed to create descriptor set layout");
    }

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .pNext = NULL,
      .descriptorPool = this->descriptorPool,
      .descriptorSetCount = 1,
      .pSetLayouts = &this->descriptorSetList[x]->getDescriptorSetLayout(),
    };

    if (vkAllocateDescriptorSets(logicalDevice, &descriptorSetAllocateInfo, &this->descriptorSetList[x]->getDescriptorSet()) != VK_SUCCESS) {
      printf("%s\n", "failed to allocate descriptor sets");
    }

    for (int y = 0; y < writeDescriptorSetList.size(); y++) {
      writeDescriptorSetList[y].dstSet = this->descriptorSetList[x]->getDescriptorSet();
    }

    vkUpdateDescriptorSets(logicalDevice, descriptorSetLayoutBindingList.size(), writeDescriptorSetList.data(), 0, NULL);
  }
}

DescriptorSetCollection::~DescriptorSetCollection() {

}

std::vector<VkDescriptorSetLayout> DescriptorSetCollection::getDescriptorSetLayoutList() {
  std::vector<VkDescriptorSetLayout> descriptorSetLayoutList;

  for (int x = 0; x < this->descriptorSetList.size(); x++) {
    descriptorSetLayoutList.push_back(this->descriptorSetList[x]->getDescriptorSetLayout());
  }

  return descriptorSetLayoutList;
}

std::vector<VkDescriptorSet> DescriptorSetCollection::getDescriptorSetList() {
  std::vector<VkDescriptorSet> descriptorSetList;

  for (int x = 0; x < this->descriptorSetList.size(); x++) {
    descriptorSetList.push_back(this->descriptorSetList[x]->getDescriptorSet());
  }

  return descriptorSetList;
}