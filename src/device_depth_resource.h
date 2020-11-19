#pragma once

#include <vulkan/vulkan.h>
#include <stdio.h>

#include "image_factory.h"

class DeviceDepthResource {
private:
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;
public:
  DeviceDepthResource(VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, VkExtent2D swapchainExtent);
  ~DeviceDepthResource();
};