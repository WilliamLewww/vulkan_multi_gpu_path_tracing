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

  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
  descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolCreateInfo.poolSizeCount = this->descriptorPoolSizeList.size();
  descriptorPoolCreateInfo.pPoolSizes = this->descriptorPoolSizeList.data();
  descriptorPoolCreateInfo.maxSets = this->deviceDescriptorSetList.size();

  if (vkCreateDescriptorPool(logicalDevice, &descriptorPoolCreateInfo, NULL, &this->descriptorPool) != VK_SUCCESS) {
    printf("%s\n", "failed to create descriptor pool");
  }

  for (int x = 0; x < this->deviceDescriptorSetList.size(); x++) {
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindingList = this->deviceDescriptorSetList[x]->getDescriptorSetLayoutBindingList();
    std::vector<VkWriteDescriptorSet> writeDescriptorSetList = this->deviceDescriptorSetList[x]->getWriteDescriptorSetList();

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = descriptorSetLayoutBindingList.size();
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindingList.data();
    
    if (vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCreateInfo, NULL, &this->deviceDescriptorSetList[x]->getDescriptorSetLayout()) != VK_SUCCESS) {
      printf("%s\n", "failed to create descriptor set layout");
    }

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = this->descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &this->deviceDescriptorSetList[x]->getDescriptorSetLayout();

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