#pragma once
#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <map>
#include <stdio.h>

#include "device_queue.h"
#include "command_pool.h"
#include "swapchain.h"
#include "render_pass.h"
#include "textures.h"
#include "framebuffers.h"
#include "model_instance_set.h"
#include "uniform_buffer_collection.h"
#include "acceleration_structure_collection.h"
#include "descriptor_set_collection.h"
#include "graphics_pipeline_collection.h"
#include "render_command_buffers.h"
#include "synchronization_objects.h"

class Device {
private:
  uint32_t currentFrame;
  uint32_t framesInFlight;

  VkPhysicalDevice physicalDevice;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  VkPhysicalDeviceFeatures physicalDeviceFeatures;
  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

  VkDevice logicalDevice;

  DeviceQueue* deviceQueue;
  CommandPool* commandPool;
  Swapchain* swapchain;
  RenderPass* renderPass;
  Textures* textures;
  Framebuffers* framebuffers;
  ModelInstanceSet* modelInstanceSet;
  UniformBufferCollection* uniformBufferCollection;
  AccelerationStructureCollection* accelerationStructureCollection;
  DescriptorSetCollection* descriptorSetCollection;
  GraphicsPipelineCollection* graphicsPipelineCollection;
  RenderCommandBuffers* renderCommandBuffers;
  SynchronizationObjects* synchronizationObjects;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();

  VkPhysicalDevice getPhysicalDevice(); 
  VkDevice getLogicalDevice();
  uint32_t getGraphicsQueueIndex(); 
  VkQueue getGraphicsQueue(); 
  uint32_t getMinImageCount();
  uint32_t getImageCount();
  VkRenderPass getRenderPass();
  VkCommandPool getCommandPool();

  UniformBufferCollection* getUniformBufferCollection();
  AccelerationStructureCollection* getAccelerationStructureCollection();
  Textures* getTextures();
  ModelInstanceSet* getModelInstanceSet();

  void initializeDeviceQueue(VkSurfaceKHR surface);
  void createLogicalDevice(std::vector<const char*> extensions);
  void createCommandPool();
  void createSwapchain(VkSurfaceKHR surface);
  void createRenderPass();
  void createTextures();
  void createFramebuffers();
  void createModelInstances(std::map<Model*, std::vector<TRS>> modelFrequencyMap);
  void createUniformBufferCollection(std::map<void*, uint32_t> bufferMap);
  void createAccelerationStructureCollection();
  void createDescriptorSetCollection(std::vector<std::vector<Descriptor*>> separatedDescriptorList);
  void createGraphicsPipelineCollection(std::vector<std::vector<std::string>> shaderList);
  void createRenderCommandBuffers();
  void createSynchronizationObjects();

  void updateAccelerationStructureCollection();

  void drawFrame();
  void updateUniformBuffer(int index, void* buffer, uint32_t bufferSize);
};