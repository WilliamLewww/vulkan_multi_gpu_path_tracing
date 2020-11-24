#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

#include "device_descriptor_set.h"

class DeviceDescriptorSetCollection {
private:
  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorPoolSize> descriptorPoolSizeList;

  std::vector<DeviceDescriptorSet*> deviceDescriptorSetList;
public:
  DeviceDescriptorSetCollection(std::vector<std::vector<DeviceDescriptor*>> separatedDeviceDescriptorList, VkDevice logicalDevice);
  ~DeviceDescriptorSetCollection();

  std::vector<VkDescriptorSetLayout> getDescriptorSetLayoutList();
};