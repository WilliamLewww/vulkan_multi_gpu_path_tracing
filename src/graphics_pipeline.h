#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class GraphicsPipeline {
private:
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
public:
  GraphicsPipeline(std::string vertexShaderFile, std::vector<VkDescriptorSetLayout> descriptorSetLayoutList, VkDevice logicalDevice, VkExtent2D swapchainExtent, VkRenderPass renderPass, uint32_t subpassIndex);
  GraphicsPipeline(std::string vertexShaderFile, std::string fragmentShaderFile, std::vector<VkDescriptorSetLayout> descriptorSetLayoutList, VkDevice logicalDevice, VkExtent2D swapchainExtent, VkRenderPass renderPass, uint32_t subpassIndex);
  GraphicsPipeline(std::string vertexShaderFile, 
                   std::vector<VkDescriptorSetLayout> descriptorSetLayoutList, 
                   VkDevice logicalDevice, 
                   VkExtent2D swapchainExtent, 
                   VkRenderPass renderPass, 
                   uint32_t subpassIndex,
                   float screenX,
                   float screenY,
                   float screenWidth,
                   float screenHeight);
  GraphicsPipeline(std::string vertexShaderFile, 
                   std::string fragmentShaderFile, 
                   std::vector<VkDescriptorSetLayout> descriptorSetLayoutList, 
                   VkDevice logicalDevice, 
                   VkExtent2D swapchainExtent, 
                   VkRenderPass renderPass, 
                   uint32_t subpassIndex,
                   float screenX,
                   float screenY,
                   float screenWidth,
                   float screenHeight);
  ~GraphicsPipeline();

  VkPipelineLayout getPipelineLayout();
  VkPipeline getGraphicsPipeline();
};