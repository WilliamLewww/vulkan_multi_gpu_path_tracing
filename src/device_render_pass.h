#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

class DeviceRenderPass {
private:
  VkRenderPass renderPass;
public:
  DeviceRenderPass(VkDevice logicalDevice, VkFormat swapchainImageFormat);
  ~DeviceRenderPass();

  VkRenderPass getRenderPass();
};