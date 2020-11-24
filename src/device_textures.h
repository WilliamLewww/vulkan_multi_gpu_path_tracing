#pragma once

#include <vulkan/vulkan.h>

#include "image_factory.h"

class DeviceTextures {
private:
  VkImageView rayTraceImageView;
  VkImage rayTraceImage;
  VkDeviceMemory rayTraceImageMemory;

  VkDescriptorImageInfo descriptorRayTraceImageInfo;
public:
  DeviceTextures(VkDevice logicalDevice, 
                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                 VkFormat swapchainImageFormat, 
                 VkCommandPool commandPool, 
                 VkQueue queue);
  
  ~DeviceTextures();

  VkImage getRayTraceImage();
  VkDescriptorImageInfo* getDescriptorRayTraceImageInfoPointer();
};