#pragma once

#include "device.h"

class DisplayDevice : public Device {
private:
  uint32_t swapchainImageCount;
  VkSwapchainKHR swapchain;
  std::vector<VkImage> swapchainImageList;
  VkFormat swapchainImageFormat;
  VkExtent2D swapchainExtent;
  std::vector<VkImageView> swapchainImageViewList;
public:
  DisplayDevice(VkPhysicalDevice physicalDevice);
  ~DisplayDevice();

  VkFormat getSwapchainImageFormat();

  void createSwapchain(VkSurfaceKHR surface);
};