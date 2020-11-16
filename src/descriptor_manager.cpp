#include "descriptor_manager.h"

DescriptorManager::DescriptorManager() {

}

DescriptorManager::~DescriptorManager() {

}

std::vector<VkDescriptorSetLayout> DescriptorManager::getDescriptorSetLayoutList(Device* device) {
  return this->deviceMap[device].descriptorSetLayoutList;
}

std::vector<VkDescriptorSet>& DescriptorManager:: getDescriptorSetListReference(Device* device) {
  return this->deviceMap[device].descriptorSetList;
}

void DescriptorManager::initializeContainerOnDevice(Device* device, int descriptorSetCount) {
  this->deviceMap.insert(std::pair<Device*, DeviceContainer>(device, DeviceContainer()));

  for (int x = 0; x < descriptorSetCount; x++) {
    this->deviceMap[device].descriptorSetList.push_back(VkDescriptorSet());
    this->deviceMap[device].descriptorSetLayoutList.push_back(VkDescriptorSetLayout());
    this->deviceMap[device].descriptorSetLayoutBindingList.push_back(std::vector<VkDescriptorSetLayoutBinding>());
    this->deviceMap[device].writeDescriptorSetList.push_back(std::vector<VkWriteDescriptorSet>());
  }
}

void DescriptorManager::addDescriptor(Device* device,
                                      int descriptorSetIndex, 
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
  this->deviceMap[device].descriptorSetLayoutBindingList[descriptorSetIndex].push_back(descriptorSetLayoutBinding);

  VkWriteDescriptorSet writeDescriptorSet = {
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .pNext = pNext,
    .dstSet = this->deviceMap[device].descriptorSetList[descriptorSetIndex],
    .dstBinding = binding,
    .dstArrayElement = 0,
    .descriptorCount = 1,
    .descriptorType = descriptorType,
    .pImageInfo = pImageInfo,
    .pBufferInfo = pBufferInfo,
    .pTexelBufferView = pTexelBufferView
  };
  this->deviceMap[device].writeDescriptorSetList[descriptorSetIndex].push_back(writeDescriptorSet);

  bool typeExists = false;
  for (int x = 0; x < this->deviceMap[device].descriptorPoolSizeList.size(); x++) {
    if (this->deviceMap[device].descriptorPoolSizeList[x].type == descriptorType) {
      this->deviceMap[device].descriptorPoolSizeList[x].descriptorCount += 1;
    }
  }

  if (!typeExists) {
    VkDescriptorPoolSize descriptorPoolSize = {
      .type = descriptorType,
      .descriptorCount = 1
    };
    this->deviceMap[device].descriptorPoolSizeList.push_back(descriptorPoolSize);
  }
}

void DescriptorManager::concludeDescriptors(Device* device) {
  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
  descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolCreateInfo.poolSizeCount = this->deviceMap[device].descriptorPoolSizeList.size();
  descriptorPoolCreateInfo.pPoolSizes = this->deviceMap[device].descriptorPoolSizeList.data();
  descriptorPoolCreateInfo.maxSets = this->deviceMap[device].descriptorSetList.size();

  if (vkCreateDescriptorPool(device->getLogicalDevice(), &descriptorPoolCreateInfo, NULL, &this->deviceMap[device].descriptorPool) != VK_SUCCESS) {
    printf("%s\n", "failed to create descriptor pool");
  }

  for (int x = 0; x < this->deviceMap[device].descriptorSetList.size(); x++) {
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = this->deviceMap[device].descriptorSetLayoutBindingList[x].size();
    descriptorSetLayoutCreateInfo.pBindings = this->deviceMap[device].descriptorSetLayoutBindingList[x].data();
    
    if (vkCreateDescriptorSetLayout(device->getLogicalDevice(), &descriptorSetLayoutCreateInfo, NULL, &this->deviceMap[device].descriptorSetLayoutList[x]) != VK_SUCCESS) {
      printf("%s\n", "failed to create descriptor set layout");
    }

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = this->deviceMap[device].descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &this->deviceMap[device].descriptorSetLayoutList[x];

    if (vkAllocateDescriptorSets(device->getLogicalDevice(), &descriptorSetAllocateInfo, &this->deviceMap[device].descriptorSetList[x]) != VK_SUCCESS) {
      printf("%s\n", "failed to allocate descriptor sets");
    }

    for (int y = 0; y < this->deviceMap[device].writeDescriptorSetList[x].size(); y++) {
      this->deviceMap[device].writeDescriptorSetList[x][y].dstSet = this->deviceMap[device].descriptorSetList[x];
    }

    vkUpdateDescriptorSets(device->getLogicalDevice(), this->deviceMap[device].descriptorSetLayoutBindingList[x].size(), this->deviceMap[device].writeDescriptorSetList[x].data(), 0, NULL);
  }
}

void DescriptorManager::print() {
  printf("==========Descriptor Manager==========\n");
  for (std::pair<Device*, DeviceContainer> pair : this->deviceMap) {
    printf("  Device: %p\n", pair.first);

    DeviceContainer deviceContainer = pair.second;
    for (int x = 0; x < deviceContainer.descriptorSetList.size(); x++) {
      printf("    Descriptor Set: %d\n", x);
      for (int y = 0; y < deviceContainer.writeDescriptorSetList[x].size(); y++) {
        VkWriteDescriptorSet writeDescriptorSet = deviceContainer.writeDescriptorSetList[x][y];
        VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = deviceContainer.descriptorSetLayoutBindingList[x][y];

        std::string typeString;
        std::string stagesString;
        const void* address;
        
        // use static map
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER) { typeString = "sampler"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) { typeString = "combined image sampler"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE) { typeString = "sampled image"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) { typeString = "storage image"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) { typeString = "uniform texel buffer"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER) { typeString = "storage texel buffer"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) { typeString = "uniform buffer"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) { typeString = "storage buffer"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) { typeString = "uniform buffer dynamic"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) { typeString = "storage buffer dynamic"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT) { typeString = "input attachment"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT) { typeString = "inline uniform block (EXT)"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) { typeString = "acceleration structure (KHR)"; }
        if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV) { typeString = "acceleration structure (NV)"; }

        if (descriptorSetLayoutBinding.stageFlags & VK_SHADER_STAGE_VERTEX_BIT) { stagesString += "vertex | "; }
        if (descriptorSetLayoutBinding.stageFlags & VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT) { stagesString += "tessellation control | "; }
        if (descriptorSetLayoutBinding.stageFlags & VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT) { stagesString += "tessellation evaluation | "; }
        if (descriptorSetLayoutBinding.stageFlags & VK_SHADER_STAGE_GEOMETRY_BIT) { stagesString += "geometry | "; }
        if (descriptorSetLayoutBinding.stageFlags & VK_SHADER_STAGE_FRAGMENT_BIT) { stagesString += "fragment | "; }
        if (descriptorSetLayoutBinding.stageFlags & VK_SHADER_STAGE_COMPUTE_BIT) { stagesString += "compute | "; }

        if (writeDescriptorSet.pImageInfo != NULL) { address = writeDescriptorSet.pImageInfo; }
        if (writeDescriptorSet.pBufferInfo != NULL) { address = writeDescriptorSet.pBufferInfo; }
        if (writeDescriptorSet.pTexelBufferView != NULL) { address = writeDescriptorSet.pTexelBufferView; }
        if (writeDescriptorSet.pNext != NULL) { address = writeDescriptorSet.pNext; }

        printf("      Binding: %d\n", writeDescriptorSet.dstBinding);
        printf("        Type: %s\n", typeString.c_str());
        printf("        Stages: %s\n", stagesString.c_str());
        printf("        Buffer: %p\n", address);
      }
    }
  }
}