#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

class DeviceFramebuffers {
private:
  std::vector<VkFramebuffer> framebufferList;
public:
  DeviceFramebuffers(VkDevice logicalDevice, 
                     uint32_t swapchainImageCount, 
                     VkExtent2D swapchainExtent,
                     std::vector<VkImageView> swapchainImageViewList,
                     VkRenderPass renderPass,
                     VkImageView depthImageView);

  ~DeviceFramebuffers();
};