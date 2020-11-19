#pragma once

#include <vulkan/vulkan.h>
#include <stdio.h>

class DeviceCommandPool {
private:
  VkCommandPool commandPool;
public:
  DeviceCommandPool(VkDevice logicalDevice, uint32_t queueIndex);
  ~DeviceCommandPool();

  VkCommandPool getCommandPool();
};