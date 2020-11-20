#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>

#include "model_instance.h"
#include "buffer_factory.h"

class ModelInstanceCollection {
private:
  std::vector<ModelInstance*> modelInstanceList;

  std::map<Model*, VkBuffer> vertexBufferMap;
  std::map<Model*, VkDeviceMemory> vertexBufferMemoryMap;

  std::map<Model*, VkBuffer> indexBufferMap;
  std::map<Model*, VkDeviceMemory> indexBufferMemoryMap;

  std::map<Model*, VkBuffer> materialIndexBufferMap;
  std::map<Model*, VkDeviceMemory> materialIndexBufferMemoryMap;

  std::map<Model*, VkBuffer> materialBufferMap;
  std::map<Model*, VkDeviceMemory> materialBufferMemoryMap;

  std::map<Model*, VkBuffer> materialLightBufferMap;
  std::map<Model*, VkDeviceMemory> materialLightBufferMemoryMap;

  VkBuffer totalVertexBuffer;
  VkDeviceMemory totalVertexBufferMemory;

  VkBuffer totalIndexBuffer;
  VkDeviceMemory totalIndexBufferMemory;

  VkBuffer totalMaterialIndexBuffer;
  VkDeviceMemory totalMaterialIndexBufferMemory;

  VkBuffer totalMaterialBuffer;
  VkDeviceMemory totalMaterialBufferMemory;

  VkBuffer totalMaterialLightBuffer;
  VkDeviceMemory totalMaterialLightBufferMemory;

  void createVertexBuffer(Model* model,
                          VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue);

  void createIndexBuffer(Model* model,
    VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue);
  void createMaterialBuffers(Model* model,
    VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue);
public:
  ModelInstanceCollection(std::map<Model*, uint32_t> modelFrequencyMap,
                          VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue);

  ~ModelInstanceCollection();
};