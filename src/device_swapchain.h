#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

class DeviceSwapchain {
private:
  uint32_t swapchainImageCount;
  VkSwapchainKHR swapchain;
  std::vector<VkImage> swapchainImageList;
  VkFormat swapchainImageFormat;
  VkExtent2D swapchainExtent;
  std::vector<VkImageView> swapchainImageViewList;
public:
  DeviceSwapchain(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t graphicsQueueIndex, uint32_t presentQueueIndex);
  ~DeviceSwapchain();
};