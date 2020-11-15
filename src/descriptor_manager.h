#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <string>
#include <stdio.h>

#include "device.h"

class DescriptorManager {
private:
  struct DeviceContainer {
    std::vector<VkDescriptorSet> descriptorSetList;
    std::vector<VkDescriptorSetLayout> descriptorSetLayoutList;

    std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindingList;
    std::vector<std::vector<VkWriteDescriptorSet>> writeDescriptorSetList;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorPoolSize> descriptorPoolSizeList;
  };
  std::map<Device*, DeviceContainer> deviceMap;
public:
  DescriptorManager();
  ~DescriptorManager();

  std::vector<VkDescriptorSetLayout> getDescriptorSetLayoutList(Device* device);
  std::vector<VkDescriptorSet>& getDescriptorSetListReference(Device* device);

  void initializeContainerOnDevice(Device* device, int descriptorSetCount);

  void addDescriptor(Device* device,
                     int descriptorSetIndex, 
                     uint32_t binding, 
                     VkDescriptorType descriptorType, 
                     VkShaderStageFlagBits stageFlags,
                     VkDescriptorImageInfo* pImageInfo = NULL, 
                     VkDescriptorBufferInfo* pBufferInfo = NULL,
                     VkBufferView* pTexelBufferView = NULL,
                     void* pNext = NULL);

  void concludeDescriptors(Device* device);

  void print();
};