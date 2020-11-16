#pragma once

#include <map>
#include <vulkan/vulkan.h>
#include <stdint.h>

#include "transformation.h"
#include "model.h"

struct ModelInstance {
  uint32_t modelIndex;
  uint32_t instanceIndex;
  Transformation transformation;

  VkBuffer vertexBuffer;
  VkBuffer indexBuffer;

  uint32_t primitiveCount;
  uint32_t vertexCount;
};

class InstanceManager {
private:
  std::map<Device*, std::vector<ModelInstance>> modelInstanceMap;
public:
  InstanceManager();
  ~InstanceManager();

  ModelInstance getInstance(Device* device, uint32_t index);
  std::vector<ModelInstance> getInstanceList(Device* device);
  std::vector<float> getTotalTransformBuffer(Device* device);
  uint32_t getInstanceCount(Device* device);

  void initializeContainerOnDevice(Device* device);

  void addInstance(Device* device, Model* model, uint32_t modelIndex, uint32_t instanceIndex);
};