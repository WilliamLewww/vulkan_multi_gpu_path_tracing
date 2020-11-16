#include "acceleration_structure_manager.h"

AccelerationStructureManager::AccelerationStructureManager() {

}

AccelerationStructureManager::~AccelerationStructureManager() {

}

VkAccelerationStructureKHR* AccelerationStructureManager::getTopLevelAccelerationStructurePointer(Device* device) {
  return &this->deviceMap[device].topLevelAccelerationStructure;
}

void AccelerationStructureManager::initializeContainerOnDevice(Device* device) {
  this->deviceMap.insert(std::pair<Device*, DeviceContainer>(device, DeviceContainer()));
}

void AccelerationStructureManager::createBottomLevelAccelerationStructure(Device* device, Model* model) {
  PFN_vkCreateAccelerationStructureKHR pvkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkCreateAccelerationStructureKHR");
  PFN_vkGetAccelerationStructureMemoryRequirementsKHR pvkGetAccelerationStructureMemoryRequirementsKHR = (PFN_vkGetAccelerationStructureMemoryRequirementsKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkGetAccelerationStructureMemoryRequirementsKHR");
  PFN_vkBindAccelerationStructureMemoryKHR pvkBindAccelerationStructureMemoryKHR = (PFN_vkBindAccelerationStructureMemoryKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkBindAccelerationStructureMemoryKHR");
  PFN_vkGetBufferDeviceAddressKHR pvkGetBufferDeviceAddressKHR = (PFN_vkGetBufferDeviceAddressKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkGetBufferDeviceAddressKHR");
  PFN_vkCmdBuildAccelerationStructureKHR pvkCmdBuildAccelerationStructureKHR = (PFN_vkCmdBuildAccelerationStructureKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkCmdBuildAccelerationStructureKHR");

  VkAccelerationStructureCreateGeometryTypeInfoKHR geometryInfos = {};
  geometryInfos.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR;
  geometryInfos.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
  geometryInfos.maxPrimitiveCount = model->getPrimitiveCount();
  geometryInfos.indexType = VK_INDEX_TYPE_UINT32;
  geometryInfos.maxVertexCount = model->getVertexCount();
  geometryInfos.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
  geometryInfos.allowsTransforms = VK_FALSE;

  VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo = {};
  accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
  accelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
  accelerationStructureCreateInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  accelerationStructureCreateInfo.maxGeometryCount = 1;
  accelerationStructureCreateInfo.pGeometryInfos = &geometryInfos;

  this->deviceMap[device].bottomLevelAccelerationStructureList.push_back(VkAccelerationStructureKHR());
  if (pvkCreateAccelerationStructureKHR(device->getLogicalDevice(), &accelerationStructureCreateInfo, NULL, &this->deviceMap[device].bottomLevelAccelerationStructureList.back()) != VK_SUCCESS) {
    printf("%s\n", "failed to create acceleration structure");
  }

  // ==============================================================================================================

  VkAccelerationStructureMemoryRequirementsInfoKHR memoryRequirementsInfo = {};
  memoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR;
  memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_KHR;
  memoryRequirementsInfo.accelerationStructure = this->deviceMap[device].bottomLevelAccelerationStructureList.back();
  memoryRequirementsInfo.buildType = VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR;

  VkMemoryRequirements2 memoryRequirements = {};
  memoryRequirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
  pvkGetAccelerationStructureMemoryRequirementsKHR(device->getLogicalDevice(), &memoryRequirementsInfo, &memoryRequirements);

  VkDeviceSize accelerationStructureSize = memoryRequirements.memoryRequirements.size;

  this->deviceMap[device].bottomLevelAccelerationStructureBufferList.push_back(VkBuffer());
  this->deviceMap[device].bottomLevelAccelerationStructureDeviceMemoryList.push_back(VkDeviceMemory());
  Buffer::createBuffer(device, accelerationStructureSize, VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->deviceMap[device].bottomLevelAccelerationStructureBufferList.back(), &this->deviceMap[device].bottomLevelAccelerationStructureDeviceMemoryList.back());

  const VkBindAccelerationStructureMemoryInfoKHR accelerationStructureMemoryInfo = {
    .sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_KHR,
    .pNext = NULL,
    .accelerationStructure = this->deviceMap[device].bottomLevelAccelerationStructureList.back(),
    .memory = this->deviceMap[device].bottomLevelAccelerationStructureDeviceMemoryList.back(),
    .memoryOffset = 0,
    .deviceIndexCount = 0,
    .pDeviceIndices = NULL
  };

  pvkBindAccelerationStructureMemoryKHR(device->getLogicalDevice(), 1, &accelerationStructureMemoryInfo);

  // ==============================================================================================================

  VkBufferDeviceAddressInfo vertexBufferDeviceAddressInfo = {};
  vertexBufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  vertexBufferDeviceAddressInfo.buffer = model->getVertexBuffer(device);

  VkDeviceAddress vertexBufferAddress = pvkGetBufferDeviceAddressKHR(device->getLogicalDevice(), &vertexBufferDeviceAddressInfo);

  VkDeviceOrHostAddressConstKHR vertexDeviceOrHostAddressConst = {};
  vertexDeviceOrHostAddressConst.deviceAddress = vertexBufferAddress;

  VkBufferDeviceAddressInfo indexBufferDeviceAddressInfo = {};
  indexBufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  indexBufferDeviceAddressInfo.buffer = model->getIndexBuffer(device);

  VkDeviceAddress indexBufferAddress = pvkGetBufferDeviceAddressKHR(device->getLogicalDevice(), &indexBufferDeviceAddressInfo);

  VkDeviceOrHostAddressConstKHR indexDeviceOrHostAddressConst = {};
  indexDeviceOrHostAddressConst.deviceAddress = indexBufferAddress;

  VkAccelerationStructureGeometryTrianglesDataKHR trianglesData = {};
  trianglesData.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
  trianglesData.pNext = NULL;
  trianglesData.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
  trianglesData.vertexData = vertexDeviceOrHostAddressConst;
  trianglesData.vertexStride = sizeof(float) * 3;
  trianglesData.indexType = VK_INDEX_TYPE_UINT32;
  trianglesData.indexData = indexDeviceOrHostAddressConst;
  trianglesData.transformData = (VkDeviceOrHostAddressConstKHR){}; 
 
  VkAccelerationStructureGeometryDataKHR geometryData = {};
  geometryData.triangles = trianglesData;

  VkAccelerationStructureGeometryKHR geometry = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
    .pNext = NULL,
    .geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR,
    .geometry = geometryData,
    .flags = VK_GEOMETRY_OPAQUE_BIT_KHR
  };
  std::vector<VkAccelerationStructureGeometryKHR> geometries = {geometry};
  VkAccelerationStructureGeometryKHR* geometriesPointer = geometries.data();

  VkBuffer scratchBuffer;
  VkDeviceMemory scratchBufferMemory;
  Buffer::createBuffer(device, accelerationStructureSize, VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &scratchBuffer, &scratchBufferMemory);

  VkBufferDeviceAddressInfo scratchBufferDeviceAddressInfo = {};
  scratchBufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  scratchBufferDeviceAddressInfo.buffer = scratchBuffer;

  VkDeviceAddress scratchBufferAddress = pvkGetBufferDeviceAddressKHR(device->getLogicalDevice(), &scratchBufferDeviceAddressInfo);

  VkDeviceOrHostAddressKHR scratchDeviceOrHostAddress = {};
  scratchDeviceOrHostAddress.deviceAddress = scratchBufferAddress;

  VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
    .pNext = NULL,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
    .flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
    .update = VK_FALSE,
    .srcAccelerationStructure = VK_NULL_HANDLE,
    .dstAccelerationStructure = this->deviceMap[device].bottomLevelAccelerationStructureList.back(),
    .geometryArrayOfPointers = VK_TRUE,
    .geometryCount = 1,
    .ppGeometries = &geometriesPointer,
    .scratchData = scratchDeviceOrHostAddress
  };

  VkAccelerationStructureBuildOffsetInfoKHR buildOffsetInfo = {
    .primitiveCount = model->getPrimitiveCount(),
    .primitiveOffset = 0,
    .firstVertex = 0,
    .transformOffset = 0  
  };
  std::vector<VkAccelerationStructureBuildOffsetInfoKHR*> buildOffsetInfos = { &buildOffsetInfo };

  VkCommandBufferAllocateInfo bufferAllocateInfo = {};
  bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  bufferAllocateInfo.commandPool = device->getCommandPool();
  bufferAllocateInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device->getLogicalDevice(), &bufferAllocateInfo, &commandBuffer);
  
  VkCommandBufferBeginInfo commandBufferBeginInfo = {};
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  
  vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
  pvkCmdBuildAccelerationStructureKHR(commandBuffer, 1, &buildGeometryInfo, buildOffsetInfos.data());
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(device->getComputeQueue(), 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(device->getComputeQueue());

  vkFreeCommandBuffers(device->getLogicalDevice(), device->getCommandPool(), 1, &commandBuffer);

  vkDestroyBuffer(device->getLogicalDevice(), scratchBuffer, NULL);
  vkFreeMemory(device->getLogicalDevice(), scratchBufferMemory, NULL);
}

void AccelerationStructureManager::createBottomLevelAccelerationStructures(Device* device, std::vector<Model*> modelList) {
  for (int x = 0; x < modelList.size(); x++) {
    this->createBottomLevelAccelerationStructure(device, modelList[x]);
  }
}

void AccelerationStructureManager::addBottomLevelAccelerationStructureInstance(Device* device, ModelInstance modelInstance) {
  PFN_vkGetAccelerationStructureDeviceAddressKHR pvkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkGetAccelerationStructureDeviceAddressKHR");

  VkAccelerationStructureDeviceAddressInfoKHR accelerationStructureDeviceAddressInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR,
    .pNext = NULL,
    .accelerationStructure = this->deviceMap[device].bottomLevelAccelerationStructureList[modelInstance.modelIndex]
  };

  VkDeviceAddress accelerationStructureDeviceAddress = pvkGetAccelerationStructureDeviceAddressKHR(device->getLogicalDevice(), &accelerationStructureDeviceAddressInfo);

  this->deviceMap[device].instanceTransformMatrixList.push_back(modelInstance.transformation.getVulkanTransformMatrix());

  VkAccelerationStructureInstanceKHR geometryInstance = {
    .transform = this->deviceMap[device].instanceTransformMatrixList.back(),
    .instanceCustomIndex = modelInstance.instanceIndex,
    .mask = 0xFF,
    .instanceShaderBindingTableRecordOffset = 0,
    .flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR,
    .accelerationStructureReference = accelerationStructureDeviceAddress
  };

  this->deviceMap[device].bottomLevelAccelerationStructureInstanceList.push_back(geometryInstance);
}

void AccelerationStructureManager::addBottomLevelAccelerationStructureInstances(Device* device, std::vector<ModelInstance> modelInstanceList) {
  for (int x = 0; x < modelInstanceList.size(); x++) {
    this->addBottomLevelAccelerationStructureInstance(device, modelInstanceList[x]);
  }
}

void AccelerationStructureManager::createTopLevelAccelerationStructure(Device* device) {
  PFN_vkCreateAccelerationStructureKHR pvkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkCreateAccelerationStructureKHR");
  PFN_vkGetAccelerationStructureMemoryRequirementsKHR pvkGetAccelerationStructureMemoryRequirementsKHR = (PFN_vkGetAccelerationStructureMemoryRequirementsKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkGetAccelerationStructureMemoryRequirementsKHR");
  PFN_vkBindAccelerationStructureMemoryKHR pvkBindAccelerationStructureMemoryKHR = (PFN_vkBindAccelerationStructureMemoryKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkBindAccelerationStructureMemoryKHR");
  PFN_vkCmdBuildAccelerationStructureKHR pvkCmdBuildAccelerationStructureKHR = (PFN_vkCmdBuildAccelerationStructureKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkCmdBuildAccelerationStructureKHR");
  PFN_vkGetBufferDeviceAddressKHR pvkGetBufferDeviceAddressKHR = (PFN_vkGetBufferDeviceAddressKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkGetBufferDeviceAddressKHR");
  PFN_vkGetAccelerationStructureDeviceAddressKHR pvkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(device->getLogicalDevice(), "vkGetAccelerationStructureDeviceAddressKHR");

  VkAccelerationStructureCreateGeometryTypeInfoKHR geometryInfos = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR,
    .pNext = NULL,
    .geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR,
    .maxPrimitiveCount = (uint32_t)this->deviceMap[device].bottomLevelAccelerationStructureInstanceList.size(),
    .indexType = VK_INDEX_TYPE_NONE_NV,
    .maxVertexCount = 0,
    .vertexFormat = VK_FORMAT_UNDEFINED,
    .allowsTransforms = VK_TRUE
  };

  VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
    .pNext = NULL,
    .compactedSize = 0,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
    .flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
    .maxGeometryCount = 1,
    .pGeometryInfos = &geometryInfos,
    .deviceAddress = VK_NULL_HANDLE
  };

  if (pvkCreateAccelerationStructureKHR(device->getLogicalDevice(), &accelerationStructureCreateInfo, NULL, &this->deviceMap[device].topLevelAccelerationStructure) != VK_SUCCESS) {
    printf("%s\n", "failed to create acceleration structure");
  }

  // ==============================================================================================================

  VkAccelerationStructureMemoryRequirementsInfoKHR memoryRequirementsInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR,
    .pNext = NULL,
    .type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_KHR,
    .buildType = VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
    .accelerationStructure = this->deviceMap[device].topLevelAccelerationStructure
  };

  VkMemoryRequirements2 memoryRequirements = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
    .pNext = NULL
  };
  
  pvkGetAccelerationStructureMemoryRequirementsKHR(device->getLogicalDevice(), &memoryRequirementsInfo, &memoryRequirements);

  VkDeviceSize accelerationStructureSize = memoryRequirements.memoryRequirements.size;

  Buffer::createBuffer(device, accelerationStructureSize, VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &this->deviceMap[device].topLevelAccelerationStructureBuffer, &this->deviceMap[device].topLevelAccelerationStructureDeviceMemory);

  const VkBindAccelerationStructureMemoryInfoKHR accelerationStructureMemoryInfo = {
    .sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_KHR,
    .pNext = NULL,
    .accelerationStructure = this->deviceMap[device].topLevelAccelerationStructure,
    .memory = this->deviceMap[device].topLevelAccelerationStructureDeviceMemory,
    .memoryOffset = 0,
    .deviceIndexCount = 0,
    .pDeviceIndices = NULL
  };

  pvkBindAccelerationStructureMemoryKHR(device->getLogicalDevice(), 1, &accelerationStructureMemoryInfo);

  // ==============================================================================================================

  VkDeviceSize geometryInstanceBufferSize = this->deviceMap[device].bottomLevelAccelerationStructureInstanceList.size() * sizeof(VkAccelerationStructureInstanceKHR);
  
  VkBuffer geometryInstanceStagingBuffer;
  VkDeviceMemory geometryInstanceStagingBufferMemory;
  Buffer::createBuffer(device, geometryInstanceBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &geometryInstanceStagingBuffer, &geometryInstanceStagingBufferMemory);

  void* geometryInstanceData;
  vkMapMemory(device->getLogicalDevice(), geometryInstanceStagingBufferMemory, 0, geometryInstanceBufferSize, 0, &geometryInstanceData);
  memcpy(geometryInstanceData, this->deviceMap[device].bottomLevelAccelerationStructureInstanceList.data(), geometryInstanceBufferSize);
  vkUnmapMemory(device->getLogicalDevice(), geometryInstanceStagingBufferMemory);

  VkBuffer geometryInstanceBuffer;
  VkDeviceMemory geometryInstanceBufferMemory;
  Buffer::createBuffer(device, geometryInstanceBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &geometryInstanceBuffer, &geometryInstanceBufferMemory);  

  Buffer::copyBuffer(device, geometryInstanceStagingBuffer, geometryInstanceBuffer, geometryInstanceBufferSize);

  vkDestroyBuffer(device->getLogicalDevice(), geometryInstanceStagingBuffer, NULL);
  vkFreeMemory(device->getLogicalDevice(), geometryInstanceStagingBufferMemory, NULL);

  VkBufferDeviceAddressInfo geometryInstanceBufferDeviceAddressInfo = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .pNext = NULL,
    .buffer = geometryInstanceBuffer
  };

  VkDeviceAddress geometryInstanceBufferAddress = pvkGetBufferDeviceAddressKHR(device->getLogicalDevice(), &geometryInstanceBufferDeviceAddressInfo);

  VkDeviceOrHostAddressConstKHR geometryInstanceDeviceOrHostAddressConst = {
    .deviceAddress = geometryInstanceBufferAddress
  };

  VkAccelerationStructureGeometryInstancesDataKHR instancesData = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
    .pNext = NULL,
    .arrayOfPointers = VK_FALSE,
    .data = geometryInstanceDeviceOrHostAddressConst 
  };

  VkAccelerationStructureGeometryDataKHR geometryData = {
    .instances = instancesData
  };

  VkAccelerationStructureGeometryKHR geometry = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
    .pNext = NULL,
    .geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR,
    .geometry = geometryData,
    .flags = 0
  };

  std::vector<VkAccelerationStructureGeometryKHR> geometries = {geometry};
  VkAccelerationStructureGeometryKHR* geometriesPointer = geometries.data();

  VkBuffer scratchBuffer;
  VkDeviceMemory scratchBufferMemory;
  Buffer::createBuffer(device, accelerationStructureSize, VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &scratchBuffer, &scratchBufferMemory);

  VkBufferDeviceAddressInfo scratchBufferDeviceAddressInfo = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .pNext = NULL,
    .buffer = scratchBuffer
  };

  VkDeviceAddress scratchBufferAddress = pvkGetBufferDeviceAddressKHR(device->getLogicalDevice(), &scratchBufferDeviceAddressInfo);

  VkDeviceOrHostAddressKHR scratchDeviceOrHostAddress = {
    .deviceAddress = scratchBufferAddress
  };

  VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
    .pNext = NULL,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
    .flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
    .update = VK_FALSE,
    .srcAccelerationStructure = VK_NULL_HANDLE,
    .dstAccelerationStructure = this->deviceMap[device].topLevelAccelerationStructure,
    .geometryArrayOfPointers = VK_FALSE,
    .geometryCount = (uint32_t)geometries.size(),
    .ppGeometries = &geometriesPointer,
    .scratchData = scratchDeviceOrHostAddress
  };

  VkAccelerationStructureBuildOffsetInfoKHR buildOffsetInfo = {
    .primitiveCount = (uint32_t)this->deviceMap[device].bottomLevelAccelerationStructureInstanceList.size(),
    .primitiveOffset = 0,
    .firstVertex = 0,
    .transformOffset = 0  
  };
  std::vector<VkAccelerationStructureBuildOffsetInfoKHR*> buildOffsetInfos = { &buildOffsetInfo };

  VkCommandBufferAllocateInfo bufferAllocateInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = NULL,
    .commandPool = device->getCommandPool(),
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1
  };

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device->getLogicalDevice(), &bufferAllocateInfo, &commandBuffer);
  
  VkCommandBufferBeginInfo commandBufferBeginInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = NULL,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    .pInheritanceInfo = NULL
  };

  vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
  pvkCmdBuildAccelerationStructureKHR(commandBuffer, 1, &buildGeometryInfo, buildOffsetInfos.data());
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = NULL,
    .waitSemaphoreCount = 0,
    .pWaitSemaphores = NULL,
    .pWaitDstStageMask = NULL,
    .commandBufferCount = 1,
    .pCommandBuffers = &commandBuffer,
    .signalSemaphoreCount = 0,
    .pSignalSemaphores = NULL
  };

  vkQueueSubmit(device->getComputeQueue(), 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(device->getComputeQueue());

  vkFreeCommandBuffers(device->getLogicalDevice(), device->getCommandPool(), 1, &commandBuffer);

  vkDestroyBuffer(device->getLogicalDevice(), scratchBuffer, NULL);
  vkFreeMemory(device->getLogicalDevice(), scratchBufferMemory, NULL);
}