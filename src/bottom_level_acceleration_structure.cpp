#include "bottom_level_acceleration_structure.h"

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(Model* model,
                                                                   VkBuffer vertexBuffer,
                                                                   VkBuffer indexBuffer,
                                                                   VkDevice logicalDevice, 
                                                                   VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                                   VkCommandPool commandPool,
                                                                   VkQueue queue) {

  PFN_vkGetAccelerationStructureBuildSizesKHR pvkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(logicalDevice, "vkGetAccelerationStructureBuildSizesKHR");
  PFN_vkCreateAccelerationStructureKHR pvkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(logicalDevice, "vkCreateAccelerationStructureKHR");
  PFN_vkCmdBuildAccelerationStructuresKHR pvkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(logicalDevice, "vkCmdBuildAccelerationStructuresKHR");

  VkAccelerationStructureGeometryTrianglesDataKHR accelerationStructureGeometryTrianglesData = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR,
    .pNext = NULL,
    .vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
    .vertexData = {},
    .vertexStride = sizeof(float) * 3,
    .maxVertex = model->getVertexCount(),
    .indexType = VK_INDEX_TYPE_UINT32,
    .indexData = {},
    .transformData = {}
  };

  VkAccelerationStructureGeometryDataKHR accelerationStructureGeometryData = {
    .triangles = accelerationStructureGeometryTrianglesData
  };

  VkAccelerationStructureGeometryKHR accelerationStructureGeometry = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
    .pNext = NULL,
    .geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR,
    .geometry = accelerationStructureGeometryData,
    .flags = VK_GEOMETRY_OPAQUE_BIT_KHR
  };

  VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
    .pNext = NULL,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
    .flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
    .mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
    .srcAccelerationStructure = VK_NULL_HANDLE,
    .dstAccelerationStructure = VK_NULL_HANDLE,
    .geometryCount = 1,
    .pGeometries = &accelerationStructureGeometry,
    .ppGeometries = NULL,
    .scratchData = {}
  };

  VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR,
    .pNext = NULL,
    .accelerationStructureSize = 0,
    .updateScratchSize = 0,
    .buildScratchSize = 0
  };

  pvkGetAccelerationStructureBuildSizesKHR(logicalDevice, 
                                           VK_ACCELERATION_STRUCTURE_BUILD_TYPE_HOST_KHR, 
                                           &accelerationStructureBuildGeometryInfo, 
                                           &accelerationStructureBuildGeometryInfo.geometryCount, 
                                           &accelerationStructureBuildSizesInfo);

  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              accelerationStructureBuildSizesInfo.accelerationStructureSize, 
                              VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &this->accelerationStructureBuffer, 
                              &this->accelerationStructureBufferMemory);

  VkBuffer scratchBuffer;
  VkDeviceMemory scratchBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              accelerationStructureBuildSizesInfo.buildScratchSize, 
                              VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &scratchBuffer, 
                              &scratchBufferMemory);

  VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
    .pNext = NULL,
    .createFlags = 0,
    .buffer = this->accelerationStructureBuffer,
    .offset = 0,
    .size = accelerationStructureBuildSizesInfo.accelerationStructureSize,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
    .deviceAddress = VK_NULL_HANDLE
  };

  pvkCreateAccelerationStructureKHR(logicalDevice, &accelerationStructureCreateInfo, NULL, &this->accelerationStructure);

  VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo = {
    .primitiveCount = model->getPrimitiveCount(),
    .primitiveOffset = 0,
    .firstVertex = 0,
    .transformOffset = 0
  };
  std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationStructureBuildRangeInfoList = { &accelerationStructureBuildRangeInfo };

  VkCommandBufferAllocateInfo bufferAllocateInfo = {};
  bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  bufferAllocateInfo.commandPool = commandPool;
  bufferAllocateInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(logicalDevice, &bufferAllocateInfo, &commandBuffer);
  
  VkCommandBufferBeginInfo commandBufferBeginInfo = {};
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  
  vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
  pvkCmdBuildAccelerationStructuresKHR(commandBuffer, 1, &accelerationStructureBuildGeometryInfo, accelerationStructureBuildRangeInfoList.data());
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);

  vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);

  vkDestroyBuffer(logicalDevice, scratchBuffer, NULL);
  vkFreeMemory(logicalDevice, scratchBufferMemory, NULL);
}

BottomLevelAccelerationStructure::~BottomLevelAccelerationStructure() {

}

VkAccelerationStructureKHR BottomLevelAccelerationStructure::getAccelerationStructure() {
  return this->accelerationStructure;
}