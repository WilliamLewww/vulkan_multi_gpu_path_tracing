#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

class Swapchain {
private:
  uint32_t swapchainImageCount;
  VkSwapchainKHR swapchain;
  std::vector<VkImage> swapchainImageList;
  VkFormat swapchainImageFormat;
  VkExtent2D swapchainExtent;
  std::vector<VkImageView> swapchainImageViewList;
public:
  Swapchain(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t graphicsQueueIndex, uint32_t presentQueueIndex);
  ~Swapchain();

  uint32_t getSwapchainImageCount();
  VkFormat getSwapchainImageFormat();
  VkExtent2D getSwapchainExtent();

  VkSwapchainKHR getSwapchain();

  std::vector<VkImageView> getSwapchainImageViewList();
  std::vector<VkImage> getSwapchainImageList();
};