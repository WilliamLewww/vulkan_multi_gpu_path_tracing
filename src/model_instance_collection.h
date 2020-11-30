#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <stddef.h>

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
    alignas(4) uint32_t count;
    alignas(4) int indicesPrimitive[64];
    alignas(4) int indicesInstance[64];
  };

  struct InstanceUniform {
    float buffer[260];
  };

  std::map<Model*, std::vector<ModelInstance*>> modelInstanceMap;

  std::vector<ModelInstance*> modelInstanceList;
  std::vector<uint32_t> vertexOffsetList;
  std::vector<uint32_t> indexOffsetList;
  std::vector<uint32_t> materialIndexOffsetList;
  std::vector<uint32_t> materialOffsetList;

  InstanceUniform instanceUniform;

  std::map<Model*, VkBuffer> vertexBufferMap;
  std::map<Model*, VkDeviceMemory> vertexBufferMemoryMap;

  std::map<Model*, VkBuffer> indexBufferMap;
  std::map<Model*, VkDeviceMemory> indexBufferMemoryMap;

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

  VkDescriptorBufferInfo descriptorTotalVertexBufferInfo;
  VkDescriptorBufferInfo descriptorTotalIndexBufferInfo;
  VkDescriptorBufferInfo descriptorTotalMaterialIndexBufferInfo;
  VkDescriptorBufferInfo descriptorTotalMaterialBufferInfo;
  VkDescriptorBufferInfo descriptorTotalMaterialLightBufferInfo;

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
                             LightContainer* totalMaterialLightList = NULL);

  void createTotalBuffers(std::vector<float> totalVertexList,
                          std::vector<uint32_t> totalIndexList,
                          std::vector<uint32_t> totalMaterialIndexList,
                          std::vector<Material> totalMaterialList,
                          LightContainer totalMaterialLightList,
                          VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue);

  std::vector<float> getTotalTransformList();
public:
  ModelInstanceCollection(std::map<Model*, std::vector<Matrix4x4>> modelFrequencyMap,
                          VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue);

  ~ModelInstanceCollection();

  void* getUniformBufferPointer();
  uint32_t getUniformBufferSize();

  std::map<Model*, std::vector<ModelInstance*>> getModelInstanceMap();

  VkDescriptorBufferInfo* getDescriptorTotalVertexBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalIndexBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalMaterialIndexBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalMaterialBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalMaterialLightBufferInfoPointer();

  std::vector<ModelInstance*> getModelInstanceList();
};