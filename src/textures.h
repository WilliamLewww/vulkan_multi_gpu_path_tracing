#pragma once

#include <vulkan/vulkan.h>

#include "image_factory.h"

class Textures {
private:
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  VkImageView rayTraceImageView;
  VkImage rayTraceImage;
  VkDeviceMemory rayTraceImageMemory;

  VkDescriptorImageInfo descriptorRayTraceImageInfo;
public:
  Textures(VkDevice logicalDevice, 
                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                 VkFormat swapchainImageFormat,
                 VkExtent2D swapchainExtent, 
                 VkCommandPool commandPool, 
                 VkQueue queue);
  
  ~Textures();

  VkImageView getDepthImageView();

  VkImage getRayTraceImage();
  VkDescriptorImageInfo* getDescriptorRayTraceImageInfoPointer();
};