#pragma once

#include "graphics_pipeline.h"

class GraphicsPipelineCollection {
private:
  std::vector<GraphicsPipeline*> graphicsPipelineList;
public:
  GraphicsPipelineCollection(std::vector<std::vector<std::string>> shaderList,
                             std::vector<VkDescriptorSetLayout> descriptorSetLayoutList,
                             VkDevice logicalDevice, 
                             VkExtent2D swapchainExtent, 
                             VkRenderPass renderPass);
  ~GraphicsPipelineCollection();

  VkPipelineLayout getPipelineLayout(int index);
  VkPipeline getGraphicsPipeline(int index);
};