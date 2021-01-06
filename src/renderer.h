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
  
  ModelInstanceSet* getModelInstanceSet(int index);

  VkPhysicalDevice getPhysicalDevice(); 
  VkDevice getLogicalDevice();
  uint32_t getGraphicsQueueIndex(); 
  VkQueue getGraphicsQueue(); 
  uint32_t getMinImageCount();
  uint32_t getImageCount();
  VkRenderPass getRenderPass();
  VkCommandPool getCommandPool();

  void render(int index);
  void updateCameraUniformBuffers(Camera* camera);
  void updateModelInstancesUniformBuffers();
  void updateAccelerationStructure(int index);
};