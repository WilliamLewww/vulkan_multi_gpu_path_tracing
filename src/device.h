#pragma once
#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <map>
#include <stdio.h>

#include "device_queue.h"
#include "command_pool.h"
#include "swapchain.h"
#include "render_pass.h"
#include "storage_buffers.h"
#include "textures.h"
#include "framebuffers.h"
#include "model_instance_set_collection.h"
#include "acceleration_structure_collection.h"
#include "uniform_buffer_collection.h"
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
  StorageBuffers* storageBuffers;
  Framebuffers* framebuffers;
  ModelInstanceSetCollection* modelInstanceSetCollection;
  UniformBufferCollection* uniformBufferCollection;
  AccelerationStructureCollection* accelerationStructureCollection;
  DescriptorSetCollection* descriptorSetCollection;
  GraphicsPipelineCollection* graphicsPipelineCollection;
  RenderCommandBuffers* renderCommandBuffers;
  SynchronizationObjects* synchronizationObjects;

  VkSampler textureSampler;
  VkDescriptorImageInfo descriptorTextureSamplerInfo;
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

  VkDescriptorImageInfo* getDescriptorTextureSamplerInfo();

  UniformBufferCollection* getUniformBufferCollection();
  AccelerationStructureSet* getAccelerationStructureSet(int index);
  Textures* getTextures();
  StorageBuffers* getStorageBuffers();

  ModelInstanceSetCollection* getModelInstanceSetCollection();
  ModelInstanceSet* getModelInstanceSet(int index);

  void initializeDeviceQueue(VkSurfaceKHR surface);
  void createLogicalDevice(std::vector<const char*> extensions);
  void createCommandPool();
  void createSwapchain(VkSurfaceKHR surface);
  void createRenderPass();
  void createTextures();
  void createSampler();
  void createStorageBuffers(uint32_t apertureInstanceIndex, uint32_t aperturePrimitiveCount, uint32_t aperturePrimitiveOffset, uint32_t lastLensElementInstanceIndex, uint32_t lastLensElementPrimitiveCount, uint32_t filmInstanceIndex);
  void createFramebuffers();
  void createModelInstanceCollection(std::vector<std::map<Model*, std::vector<TRS>>> modelFrequencyMapList);
  void createUniformBufferCollection(std::map<void*, uint32_t> bufferMap);
  void createAccelerationStructureCollection();
  void createDescriptorSetCollection(std::vector<std::vector<Descriptor*>> separatedDescriptorList);
  void createGraphicsPipelineCollection(std::vector<std::vector<std::string>> shaderList, std::vector<std::vector<float>> screenPropertiesList);
  void createRenderCommandBuffers();
  void createSynchronizationObjects();

  void updateAccelerationStructureSet(int index);

  void drawFrame(int index);
  void updateUniformBuffer(int index, void* buffer, uint32_t bufferSize);
};