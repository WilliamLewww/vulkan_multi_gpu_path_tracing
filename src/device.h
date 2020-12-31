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
#include "model_instance_collection.h"
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
  ModelInstanceCollection* modelInstanceCollection;
  UniformBufferCollection* uniformBufferCollection;
  AccelerationStructureCollection* accelerationStructureCollection;
  DescriptorSetCollection* descriptorSetCollection;
  GraphicsPipelineCollection* graphicsPipelineCollection;
  RenderCommandBuffers* renderCommandBuffers;
  SynchronizationObjects* synchronizationObjects;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();

  ModelInstanceCollection* getModelInstanceCollectionPointer();
  UniformBufferCollection* getUniformBufferCollection();
  AccelerationStructureCollection* getAccelerationStructureCollection();
  Textures* getTextures();
  ModelInstanceCollection* getModelInstanceCollection();

  void initializeDeviceQueue(VkSurfaceKHR surface);
  void createLogicalDevice(std::vector<const char*> extensions);
  void createCommandPool();
  void createSwapchain(VkSurfaceKHR surface);
  void createRenderPass();
  void createTextures();
  void createFramebuffers();
  void createModelInstances(std::map<Model*, std::vector<Matrix4x4>> modelFrequencyMap);
  void createUniformBufferCollection(std::map<void*, uint32_t> bufferMap);
  void createAccelerationStructureCollection(std::map<Model*, std::vector<ModelInstance*>> modelInstanceMap);
  void createDescriptorSetCollection(std::vector<std::vector<Descriptor*>> separatedDescriptorList);
  void createGraphicsPipeline(std::vector<std::vector<std::string>> shaderList);
  void createRenderCommandBuffers();
  void createSynchronizationObjects();

  void drawFrame();
  void updateUniformBuffer(int index, void* buffer, uint32_t bufferSize);
};