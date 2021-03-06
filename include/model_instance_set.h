#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <stddef.h>

#include "stb/stb_image.h"
#include "model_instance.h"
#include "buffer_factory.h"
#include "image_factory.h"

class ModelInstanceSet {
private:
  struct Material {
    alignas(16) float ambient[3];
    alignas(16) float diffuse[3];
    alignas(16) float specular[3];
    alignas(16) float emission[3];

    alignas(4) float shininess;
    alignas(4) float dissolve;
    alignas(4) float ior;

    alignas(4) int diffuseTextureIndex;
    alignas(4) int type;
  };

  struct LightContainer {
    alignas(4) uint32_t count;
    alignas(4) uint32_t indicesPrimitive[64];
    alignas(4) uint32_t indicesInstance[64];
  };

  struct InstanceUniform {
    float buffer[324];
  };

  std::map<Model*, std::vector<ModelInstance*>> modelInstanceMap;

  std::vector<ModelInstance*> modelInstanceList;

  std::vector<uint32_t> vertexOffsetList;
  std::vector<uint32_t> normalOffsetList;
  std::vector<uint32_t> textureCoordinateOffsetList;
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

  VkBuffer totalNormalBuffer;
  VkDeviceMemory totalNormalBufferMemory;

  VkBuffer totalTextureCoordinateBuffer;
  VkDeviceMemory totalTextureCoordinateBufferMemory;

  VkBuffer totalIndexBuffer;
  VkDeviceMemory totalIndexBufferMemory;

  VkBuffer totalNormalIndexBuffer;
  VkDeviceMemory totalNormalIndexBufferMemory;

  VkBuffer totalTextureCoordinateIndexBuffer;
  VkDeviceMemory totalTextureCoordinateIndexBufferMemory;

  VkBuffer totalMaterialIndexBuffer;
  VkDeviceMemory totalMaterialIndexBufferMemory;

  VkBuffer totalMaterialBuffer;
  VkDeviceMemory totalMaterialBufferMemory;

  VkBuffer totalMaterialLightBuffer;
  VkDeviceMemory totalMaterialLightBufferMemory;

  std::vector<VkImage> materialImageList;
  std::vector<VkImageView> materialImageViewList;
  std::vector<VkDeviceMemory> materialImageMemory;

  std::vector<VkDescriptorImageInfo> descriptorMaterialImageInfoList;

  VkDescriptorBufferInfo descriptorTotalVertexBufferInfo;
  VkDescriptorBufferInfo descriptorTotalNormalBufferInfo;
  VkDescriptorBufferInfo descriptorTotalTextureCoordinateBufferInfo;
  VkDescriptorBufferInfo descriptorTotalIndexBufferInfo;
  VkDescriptorBufferInfo descriptorTotalNormalIndexBufferInfo;
  VkDescriptorBufferInfo descriptorTotalTextureCoordinateIndexBufferInfo;
  VkDescriptorBufferInfo descriptorTotalMaterialIndexBufferInfo;
  VkDescriptorBufferInfo descriptorTotalMaterialBufferInfo;
  VkDescriptorBufferInfo descriptorTotalMaterialLightBufferInfo;

  void createVertexBuffer(Model* model,
                          VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue,
                          std::vector<float>* totalVertexList = NULL,
                          std::vector<float>* totalNormalList = NULL,
                          std::vector<float>* totalTextureCoordinateList = NULL);

  void createIndexBuffer(Model* model,
                         VkDevice logicalDevice, 
                         VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                         VkCommandPool commandPool,
                         VkQueue queue,
                         std::vector<uint32_t>* totalIndexList = NULL,
                         std::vector<uint32_t>* totalNormalIndexList = NULL,
                         std::vector<uint32_t>* totalTextureCoordinateIndexList = NULL);

  void createMaterialBuffers(Model* model,
                             VkDevice logicalDevice, 
                             VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                             VkCommandPool commandPool,
                             VkQueue queue,
                             std::vector<uint32_t>* totalMaterialIndexList = NULL,
                             std::vector<Material>* totalMaterialList = NULL,
                             std::vector<std::string>* totalTextureNameList = NULL,
                             LightContainer* totalMaterialLightList = NULL);

  void createTotalBuffers(std::vector<float> totalVertexList,
                          std::vector<float> totalNormalList,
                          std::vector<float> totalTextureCoordinateList,
                          std::vector<uint32_t> totalIndexList,
                          std::vector<uint32_t> totalNormalIndexList,
                          std::vector<uint32_t> totalTextureCoordinateIndexList,
                          std::vector<uint32_t> totalMaterialIndexList,
                          std::vector<Material> totalMaterialList,
                          LightContainer totalMaterialLightList,
                          VkDevice logicalDevice, 
                          VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                          VkCommandPool commandPool,
                          VkQueue queue);

  std::vector<float> getTotalTransformList();
public:
  ModelInstanceSet(std::map<Model*, std::vector<TRS>> modelFrequencyMap,
                   VkDevice logicalDevice, 
                   VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                   VkCommandPool commandPool,
                   VkQueue queue);

  ~ModelInstanceSet();

  void* getUniformBufferPointer();
  uint32_t getUniformBufferSize();

  uint32_t getModelInstanceIndex(Model* model);

  uint32_t getInstanceCount();
  ModelInstance* getModelInstance(int index);

  std::map<Model*, std::vector<ModelInstance*>> getModelInstanceMap();

  VkDescriptorBufferInfo* getDescriptorTotalVertexBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalNormalBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalIndexBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalNormalIndexBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalMaterialIndexBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalMaterialBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalMaterialLightBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalTextureCoordinateBufferInfoPointer();
  VkDescriptorBufferInfo* getDescriptorTotalTextureCoordinateIndexBufferInfoPointer();

  VkDescriptorImageInfo* getDescriptorMaterialImageInfoListPointer();

  std::vector<ModelInstance*> getModelInstanceList();

  void updateUniformBuffer();
};