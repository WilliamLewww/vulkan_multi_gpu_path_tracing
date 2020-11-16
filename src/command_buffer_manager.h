#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>

#include "device.h"
#include "instance_manager.h"

class CommandBufferManager {
private:
  struct DeviceContainer {
    std::vector<VkCommandBuffer> commandBufferList;
  };
  std::map<Device*, DeviceContainer> deviceMap;
public:
  CommandBufferManager();
  ~CommandBufferManager();

  std::vector<VkCommandBuffer> getCommandBufferList(Device* device);

  void initializeContainerOnDevice(Device* device);
  void createCommandBuffers(Device* device,
                            std::vector<ModelInstance> modelInstanceList, 
                            VkPipeline pipeline, 
                            VkPipelineLayout pipelineLayout, 
                            std::vector<VkDescriptorSet>& descriptorSetList);
};