#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <map>

#include "device.h"

class GraphicsPipeline {
private:
  FILE* vertexFile;
  uint32_t vertexFileSize;
  char* vertexFileBuffer;

  FILE* fragmentFile;
  uint32_t fragmentFileSize;
  char* fragmentFileBuffer;

  struct DeviceContainer {
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
  };
  std::map<Device*, DeviceContainer> deviceMap;
public:
  GraphicsPipeline();
  ~GraphicsPipeline();

  VkPipelineLayout getPipelineLayout(Device* device);
  VkPipeline getPipeline(Device* device);

  void setVertexFile(std::string path);
  void setFragmentFile(std::string path);

  void initializeContainerOnDevice(Device* device);
  void createPipelineLayout(Device* device, std::vector<VkDescriptorSetLayout> descriptorSetLayoutList);
  void createPipeline(Device* device,
                              std::vector<VkVertexInputBindingDescription> vertexBindingDescriptionList,
                              std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptionList,
                              VkExtent2D swapchainExtent,
                              VkRenderPass renderPass);
};