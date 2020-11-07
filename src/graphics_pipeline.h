#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class GraphicsPipeline {
private:
  FILE* vertexFile;
  uint32_t vertexFileSize;
  char* vertexFileBuffer;

  FILE* fragmentFile;
  uint32_t fragmentFileSize;
  char* fragmentFileBuffer;

  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
public:
  GraphicsPipeline();
  ~GraphicsPipeline();

  void setVertexFile(std::string path);
  void setFragmentFile(std::string path);

  void createPipelineLayout(VkDevice logicalDevice, std::vector<VkDescriptorSetLayout> descriptorSetLayoutList);
  void createGraphicsPipeline(VkDevice logicalDevice,
                              std::vector<VkVertexInputBindingDescription> vertexBindingDescriptionList,
                              std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptionList,
                              VkExtent2D swapchainExtent,
                              VkRenderPass renderPass);
};