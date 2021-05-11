#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

class Framebuffers {
private:
  std::vector<VkFramebuffer> framebufferList;
public:
  Framebuffers(VkDevice logicalDevice, 
                     uint32_t swapchainImageCount, 
                     VkExtent2D swapchainExtent,
                     std::vector<VkImageView> swapchainImageViewList,
                     VkRenderPass renderPass,
                     VkImageView depthImageView);

  ~Framebuffers();

  std::vector<VkFramebuffer> getFramebufferList();
};