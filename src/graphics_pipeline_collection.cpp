#include "graphics_pipeline_collection.h"

GraphicsPipelineCollection::GraphicsPipelineCollection(std::vector<std::vector<std::string>> shaderList,
                                                       std::vector<VkDescriptorSetLayout> descriptorSetLayoutList,
                                                       VkDevice logicalDevice, 
                                                       VkExtent2D swapchainExtent, 
                                                       VkRenderPass renderPass) {

  for (int x = 0; x < shaderList.size(); x++) {
    if (shaderList[x].size() == 1) {
      this->graphicsPipelineList.push_back(new GraphicsPipeline(shaderList[x][0], 
                                                                descriptorSetLayoutList, 
                                                                logicalDevice, 
                                                                swapchainExtent, 
                                                                renderPass,
                                                                x));
    }

    if (shaderList[x].size() == 2) {
      this->graphicsPipelineList.push_back(new GraphicsPipeline(shaderList[x][0], 
                                                                shaderList[x][1], 
                                                                descriptorSetLayoutList, 
                                                                logicalDevice, 
                                                                swapchainExtent, 
                                                                renderPass,
                                                                x));
    }
  }
}

GraphicsPipelineCollection::~GraphicsPipelineCollection() {

}

std::vector<VkPipelineLayout> GraphicsPipelineCollection::getPipelineLayoutList() {
  std::vector<VkPipelineLayout> pipelineLayoutList;

  for (int x = 0; x < this->graphicsPipelineList.size(); x++) {
    pipelineLayoutList.push_back(this->graphicsPipelineList[x]->getPipelineLayout());
  }

  return pipelineLayoutList;
}

std::vector<VkPipeline> GraphicsPipelineCollection::getGraphicsPipelineList() {
  std::vector<VkPipeline> pipelineList;

  for (int x = 0; x < this->graphicsPipelineList.size(); x++) {
    pipelineList.push_back(this->graphicsPipelineList[x]->getGraphicsPipeline());
  }

  return pipelineList;
}