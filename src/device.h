#pragma once
#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <map>
#include <stdio.h>

#include "device_queue.h"
#include "device_command_pool.h"
#include "device_swapchain.h"
#include "device_render_pass.h"
#include "device_textures.h"
#include "device_framebuffers.h"
#include "model_instance_collection.h"
#include "device_uniform_buffer_collection.h"
#include "acceleration_structure_collection.h"
#include "device_descriptor_set_collection.h"
#include "graphics_pipeline.h"
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

  std::vector<GraphicsPipeline*> graphicsPipelineList;

  DeviceQueue* deviceQueue;
  DeviceCommandPool* deviceCommandPool;
  DeviceSwapchain* deviceSwapchain;
  DeviceRenderPass* deviceRenderPass;
  DeviceFramebuffers* deviceFramebuffers;
  DeviceTextures* deviceTextures;
  ModelInstanceCollection* modelInstanceCollection;
  DeviceUniformBufferCollection* deviceUniformBufferCollection;
  AccelerationStructureCollection* accelerationStructureCollection;
  DeviceDescriptorSetCollection* deviceDescriptorSetCollection;
  RenderCommandBuffers* renderCommandBuffers;
  SynchronizationObjects* synchronizationObjects;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();

  ModelInstanceCollection* getModelInstanceCollectionPointer();
  DeviceUniformBufferCollection* getDeviceUniformBufferCollection();
  AccelerationStructureCollection* getAccelerationStructureCollection();
  DeviceTextures* getDeviceTextures();
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
  void createDescriptorSetCollection(std::vector<std::vector<DeviceDescriptor*>> separatedDeviceDescriptorList);
  void createGraphicsPipeline(std::string vertexShaderFile);
  void createGraphicsPipeline(std::string vertexShaderFile, std::string fragmentShaderFile);
  void createRenderCommandBuffers();
  void createSynchronizationObjects();

  void drawFrame();
  void updateUniformBuffer(int index, void* buffer, uint32_t bufferSize);
};