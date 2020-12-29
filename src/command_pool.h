#pragma once

#include <vulkan/vulkan.h>
#include <stdio.h>

class CommandPool {
private:
  VkCommandPool commandPool;
public:
  CommandPool(VkDevice logicalDevice, uint32_t queueIndex);
  ~CommandPool();

  VkCommandPool getCommandPool();
};