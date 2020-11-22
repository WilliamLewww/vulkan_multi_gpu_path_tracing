#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>

#include "model_instance.h"
#include "buffer_factory.h"

class ModelInstanceCollection {
private:
  struct Material {
    alignas(16) float ambient[3];
    alignas(16) float diffuse[3];
    alignas(16) float specular[3];
    alignas(16) float emission[3];
  };

  struct LightContainer {
    alignas(4) int count;
    alignas(4) int indices[64];
  };

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
                          VkQueue queue,
                          std::vector<float>* totalVertexList = NULL);

  void createIndexBuffer(Model* model,
                         VkDevice logicalDevice, 
                         VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                         VkCommandPool commandPool,
                         VkQueue queue,
                         std::vector<uint32_t>* totalIndexList = NULL);

  void createMaterialBuffers(Model* model,
                             VkDevice logicalDevice, 
                             VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                             VkCommandPool commandPool,
                             VkQueue queue,
                             std::vector<uint32_t>* totalMaterialIndexList = NULL,
                             std::vector<Material>* totalMaterialList = NULL,
                             std::vector<LightContainer>* totalMaterialLightList = NULL);
public:
  ModelInstanceCollection(std::map<Model*, uint32_t> modelFrequencyMap,
                          VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue);

  ~ModelInstanceCollection();
};