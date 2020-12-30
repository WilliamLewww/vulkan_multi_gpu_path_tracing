#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

class RenderPass {
private:
  VkRenderPass renderPass;
public:
  RenderPass(VkDevice logicalDevice, VkFormat swapchainImageFormat);
  ~RenderPass();

  VkRenderPass getRenderPass();
};