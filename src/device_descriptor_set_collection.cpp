#include "device_descriptor_set_collection.h"

DeviceDescriptorSetCollection::DeviceDescriptorSetCollection(std::vector<std::vector<DeviceDescriptor*>> separatedDeviceDescriptorList, VkDevice logicalDevice) {
  for (int x = 0; x < separatedDeviceDescriptorList.size(); x++) {
    this->deviceDescriptorSetList.push_back(new DeviceDescriptorSet(separatedDeviceDescriptorList[x]));
    
    for (int y = 0; y < separatedDeviceDescriptorList[x].size(); y++) {
      bool typeExists = false;
      for (int z = 0; z < this->descriptorPoolSizeList.size(); z++) {
        if (this->descriptorPoolSizeList[z].type == separatedDeviceDescriptorList[x][y]->getDescriptorType()) {
          this->descriptorPoolSizeList[z].descriptorCount += 1;
          typeExists = true;
        }
      }

      if (!typeExists) {
        VkDescriptorPoolSize descriptorPoolSize = {
          .type = separatedDeviceDescriptorList[x][y]->getDescriptorType(),
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
    .maxSets = (uint32_t)this->deviceDescriptorSetList.size(),
    .poolSizeCount = (uint32_t)this->descriptorPoolSizeList.size(),
    .pPoolSizes = this->descriptorPoolSizeList.data()
  };

  if (vkCreateDescriptorPool(logicalDevice, &descriptorPoolCreateInfo, NULL, &this->descriptorPool) != VK_SUCCESS) {
    printf("%s\n", "failed to create descriptor pool");
  }

  for (int x = 0; x < this->deviceDescriptorSetList.size(); x++) {
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindingList = this->deviceDescriptorSetList[x]->getDescriptorSetLayoutBindingList();
    std::vector<VkWriteDescriptorSet> writeDescriptorSetList = this->deviceDescriptorSetList[x]->getWriteDescriptorSetList();

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .bindingCount = (uint32_t)descriptorSetLayoutBindingList.size(),
      .pBindings = descriptorSetLayoutBindingList.data(),
    };
    
    if (vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCreateInfo, NULL, &this->deviceDescriptorSetList[x]->getDescriptorSetLayout()) != VK_SUCCESS) {
      printf("%s\n", "failed to create descriptor set layout");
    }

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .pNext = NULL,
      .descriptorPool = this->descriptorPool,
      .descriptorSetCount = 1,
      .pSetLayouts = &this->deviceDescriptorSetList[x]->getDescriptorSetLayout(),
    };

    if (vkAllocateDescriptorSets(logicalDevice, &descriptorSetAllocateInfo, &this->deviceDescriptorSetList[x]->getDescriptorSet()) != VK_SUCCESS) {
      printf("%s\n", "failed to allocate descriptor sets");
    }

    for (int y = 0; y < writeDescriptorSetList.size(); y++) {
      writeDescriptorSetList[y].dstSet = this->deviceDescriptorSetList[x]->getDescriptorSet();
    }

    vkUpdateDescriptorSets(logicalDevice, descriptorSetLayoutBindingList.size(), writeDescriptorSetList.data(), 0, NULL);
  }
}

DeviceDescriptorSetCollection::~DeviceDescriptorSetCollection() {

}

std::vector<VkDescriptorSetLayout> DeviceDescriptorSetCollection::getDescriptorSetLayoutList() {
  std::vector<VkDescriptorSetLayout> descriptorSetLayoutList;

  for (int x = 0; x < this->deviceDescriptorSetList.size(); x++) {
    descriptorSetLayoutList.push_back(this->deviceDescriptorSetList[x]->getDescriptorSetLayout());
  }

  return descriptorSetLayoutList;
}

std::vector<VkDescriptorSet> DeviceDescriptorSetCollection::getDescriptorSetList() {
  std::vector<VkDescriptorSet> descriptorSetList;

  for (int x = 0; x < this->deviceDescriptorSetList.size(); x++) {
    descriptorSetList.push_back(this->deviceDescriptorSetList[x]->getDescriptorSet());
  }

  return descriptorSetList;
}