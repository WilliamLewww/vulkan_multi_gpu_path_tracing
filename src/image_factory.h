#pragma once

#include <vulkan/vulkan.h>
#include <stdio.h>

class ImageFactory {
public:
  static void createImage(VkDevice logicalDevice,
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties,
                          uint32_t width, 
                          uint32_t height, 
                          VkFormat format, 
                          VkImageTiling tiling, 
                          VkImageUsageFlags usageFlags, 
                          VkMemoryPropertyFlags propertyFlags, 
                          VkImage* image, 
                          VkDeviceMemory* imageMemory);
};