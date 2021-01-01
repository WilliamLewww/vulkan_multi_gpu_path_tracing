#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

#include "model_instance.h"

class RenderCommandBuffers {
private:
  std::vector<VkCommandBuffer> commandBufferList;
public:
  RenderCommandBuffers(VkDevice logicalDevice, 
                       uint32_t swapchainImageCount, 
                       VkCommandPool commandPool);

  ~RenderCommandBuffers();

  std::vector<VkCommandBuffer>& getCommandBufferList();

  void recreateCommandBuffer(uint32_t imageIndex,
                             VkExtent2D swapchainExtent, 
                             VkRenderPass renderPass, 
                             std::vector<VkFramebuffer> framebufferList,
                             std::vector<VkImage> swapchainImageList,
                             VkImage rayTraceImage,
                             std::vector<VkPipeline> pipelineList,
                             std::vector<VkPipelineLayout> pipelineLayoutList,
                             std::vector<VkDescriptorSet> descriptorSetList,
                             std::vector<ModelInstance*> modelInstanceList);
};