#pragma once

#include <map>

#include "device_collection.h"
#include "model_collection.h"
#include "camera.h"

class Renderer {
private:
  DeviceCollection* deviceCollection;
  Device* displayDevice;
public:
  Renderer(VkInstance vulkanInstance, VkSurfaceKHR surface, ModelCollection* modelCollection, Camera* camera);
  ~Renderer();

  VkPhysicalDevice getPhysicalDevice(); 
  VkDevice getLogicalDevice();
  uint32_t getGraphicsQueueIndex(); 
  VkQueue getGraphicsQueue(); 
  VkDescriptorPool getDescriptorPool();
  uint32_t getMinImageCount();
  uint32_t getImageCount();
  VkRenderPass getRenderPass();

  void render();
  void updateUniformBuffers(Camera* camera);
};