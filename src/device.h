#pragma once

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdio.h>

class Device {
protected:
  VkPhysicalDevice physicalDevice;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  VkPhysicalDeviceFeatures physicalDeviceFeatures;
  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

  uint32_t graphicsQueueIndex;
  uint32_t presentQueueIndex;
  uint32_t computeQueueIndex;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VkQueue computeQueue;

  VkDevice logicalDevice;
  VkCommandPool commandPool;
  VkRenderPass renderPass;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();

  void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage* image, VkDeviceMemory* imageMemory);

  void initializeQueues(VkSurfaceKHR surface);
  void createLogicalDevice(std::vector<const char*> extensions);
  void createCommandPool();
  void createRenderPass(VkFormat format);
  void createDepthResource(uint32_t width, uint32_t height);
};