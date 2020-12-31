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
                                                                renderPass));
    }

    if (shaderList[x].size() == 2) {
      this->graphicsPipelineList.push_back(new GraphicsPipeline(shaderList[x][0], 
                                                                shaderList[x][1], 
                                                                descriptorSetLayoutList, 
                                                                logicalDevice, 
                                                                swapchainExtent, 
                                                                renderPass));
    }
  }
}

GraphicsPipelineCollection::~GraphicsPipelineCollection() {

}

VkPipelineLayout GraphicsPipelineCollection::getPipelineLayout(int index) {
  return this->graphicsPipelineList[index]->getPipelineLayout();
}

VkPipeline GraphicsPipelineCollection::getGraphicsPipeline(int index) {
  return this->graphicsPipelineList[index]->getGraphicsPipeline();
}