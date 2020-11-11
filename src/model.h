#pragma once

#include <vulkan/vulkan.h>
#include <map>
#include <stdio.h>
#include <unistd.h>

#include "tiny_obj_loader.h"
#include "device.h"
#include "buffer.h"

struct Material {
  float ambient[3]; int padA;
  float diffuse[3]; int padB;
  float specular[3]; int padC;
  float emission[3]; int padD;
};

struct LightContainer {
  int count;
  int indices[64];
};

class Model {
private:
  std::string fileName;
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warning;
  std::string error;
  bool success;

  struct DeviceContainer {
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkBuffer materialIndexBuffer;
    VkDeviceMemory materialIndexBufferMemory;

    VkBuffer materialBuffer;
    VkDeviceMemory materialBufferMemory;

    VkBuffer materialLightBuffer;
    VkDeviceMemory materialLightBufferMemory;
  };
  std::map<Device*, DeviceContainer> deviceMap;
public:
  Model(std::string fileName);
  ~Model();

  bool checkError();

  uint32_t getVertexCount();
  std::vector<float> getVertices();

  uint32_t getTotalIndexCount();
  tinyobj::index_t getTotalIndex(uint32_t index);

  uint32_t getTotalMaterialIndexCount();
  int getTotalMaterialIndex(uint32_t index);

  uint32_t getMaterialCount();
  tinyobj::material_t getMaterial(uint32_t index);

  uint32_t getPrimitiveCount();

  VkBuffer getVertexBuffer(Device* device);
  VkBuffer getIndexBuffer(Device* device);
  VkBuffer getMaterialIndexBuffer(Device* device);
  VkBuffer getMaterialBuffer(Device* device);
  VkBuffer getMaterialLightBuffer(Device* device);

  void createVertexBuffer(Device* device);
  void createIndexBuffer(Device* device);
  void createMaterialBuffers(Device* device);
};