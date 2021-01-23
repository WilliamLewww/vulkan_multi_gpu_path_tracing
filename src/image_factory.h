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

  static void copyBufferToImage(VkDevice logicalDevice,
                                VkCommandPool commandPool,
                                VkQueue queue,
                                VkBuffer srcBuffer,
                                VkImage dstImage, 
                                uint32_t width,
                                uint32_t height);

  static void transitionImageLayout(VkDevice logicalDevice,
                                    VkCommandPool commandPool,
                                    VkQueue queue,
                                    VkImage image,
                                    VkFormat format, 
                                    VkImageLayout oldLayout,
                                    VkImageLayout newLayout);

  static void createImageView(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageView* imageView);
};