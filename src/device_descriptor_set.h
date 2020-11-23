#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>

#include "device_descriptor.h"

class DeviceDescriptorSet {
private:
  VkDescriptorSet descriptorSet;
  VkDescriptorSetLayout descriptorSetLayout;

  std::vector<DeviceDescriptor*> deviceDescriptorList;
public:
  DeviceDescriptorSet(std::vector<DeviceDescriptor*> deviceDescriptorList);
  ~DeviceDescriptorSet();
};