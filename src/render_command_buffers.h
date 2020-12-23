#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

#include "model_instance.h"

class RenderCommandBuffers {
private:
  std::vector<VkCommandBuffer> commandBufferList;
public:
  RenderCommandBuffers(VkDevice logicalDevice, 
                      uint32_t swapchainImageCount, 
                      VkExtent2D swapchainExtent, 
                      VkCommandPool commandPool, 
                      VkRenderPass renderPass, 
                      std::vector<VkFramebuffer> framebufferList,
                      std::vector<VkImage> swapchainImageList,
                      VkImage rayTraceImage,
                      VkPipeline depthPipeline,
                      VkPipeline pipeline,
                      VkPipelineLayout pipelineLayout,
                      std::vector<VkDescriptorSet> descriptorSetList,
                      std::vector<ModelInstance*> modelInstanceList);

  ~RenderCommandBuffers();

  std::vector<VkCommandBuffer>& getCommandBufferList();
};