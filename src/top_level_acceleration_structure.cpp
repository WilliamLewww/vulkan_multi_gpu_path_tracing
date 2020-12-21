#include "top_level_acceleration_structure.h"

TopLevelAccelerationStructure::TopLevelAccelerationStructure(std::vector<VkAccelerationStructureInstanceKHR> bottomLevelAccelerationStructureInstanceList,
                                                             VkDevice logicalDevice, 
                                                             VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                             VkCommandPool commandPool,
                                                             VkQueue queue) {

  PFN_vkGetAccelerationStructureBuildSizesKHR pvkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(logicalDevice, "vkGetAccelerationStructureBuildSizesKHR");
  PFN_vkCreateAccelerationStructureKHR pvkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(logicalDevice, "vkCreateAccelerationStructureKHR");
  PFN_vkGetBufferDeviceAddressKHR pvkGetBufferDeviceAddressKHR = (PFN_vkGetBufferDeviceAddressKHR)vkGetDeviceProcAddr(logicalDevice, "vkGetBufferDeviceAddressKHR");
  PFN_vkCmdBuildAccelerationStructuresKHR pvkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(logicalDevice, "vkCmdBuildAccelerationStructuresKHR");

  VkDeviceSize geometryInstanceBufferSize = bottomLevelAccelerationStructureInstanceList.size() * sizeof(VkAccelerationStructureInstanceKHR);
  
  VkBuffer geometryInstanceStagingBuffer;
  VkDeviceMemory geometryInstanceStagingBufferMemory;
  BufferFactory::createBuffer(logicalDevice,
                              physicalDeviceMemoryProperties,
                              geometryInstanceBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              &geometryInstanceStagingBuffer, 
                              &geometryInstanceStagingBufferMemory);

  void* geometryInstanceData;
  vkMapMemory(logicalDevice, geometryInstanceStagingBufferMemory, 0, geometryInstanceBufferSize, 0, &geometryInstanceData);
  memcpy(geometryInstanceData, bottomLevelAccelerationStructureInstanceList.data(), geometryInstanceBufferSize);
  vkUnmapMemory(logicalDevice, geometryInstanceStagingBufferMemory);

  VkBuffer geometryInstanceBuffer;
  VkDeviceMemory geometryInstanceBufferMemory;
  BufferFactory::createBuffer(logicalDevice, 
                              physicalDeviceMemoryProperties, 
                              geometryInstanceBufferSize, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              &geometryInstanceBuffer, 
                              &geometryInstanceBufferMemory);  

  BufferFactory::copyBuffer(logicalDevice, 
                            commandPool, 
                            queue, 
                            geometryInstanceStagingBuffer, 
                            geometryInstanceBuffer, 
                            geometryInstanceBufferSize);

  vkDestroyBuffer(logicalDevice, geometryInstanceStagingBuffer, NULL);
  vkFreeMemory(logicalDevice, geometryInstanceStagingBufferMemory, NULL);

  VkBufferDeviceAddressInfo geometryInstanceBufferDeviceAddressInfo = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .pNext = NULL,
    .buffer = geometryInstanceBuffer
  };

  VkDeviceAddress geometryInstanceBufferAddress = pvkGetBufferDeviceAddressKHR(logicalDevice, &geometryInstanceBufferDeviceAddressInfo);

  VkDeviceOrHostAddressConstKHR geometryInstanceDeviceOrHostAddressConst = {
    .deviceAddress = geometryInstanceBufferAddress
  };

  VkAccelerationStructureGeometryInstancesDataKHR accelerationStructureGeometryInstancesData = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
    .pNext = NULL,
    .arrayOfPointers = VK_FALSE,
    .data = geometryInstanceDeviceOrHostAddressConst
  };

  VkAccelerationStructureGeometryDataKHR accelerationStructureGeometryData = {
    .instances = accelerationStructureGeometryInstancesData
  };

  VkAccelerationStructureGeometryKHR accelerationStructureGeometry = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
    .pNext = NULL,
    .geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR,
    .geometry = accelerationStructureGeometryData,
    .flags = VK_GEOMETRY_OPAQUE_BIT_KHR
  };

  VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
    .pNext = NULL,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
    .flags = 0,
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

  VkBufferDeviceAddressInfo scratchBufferDeviceAddressInfo = {};
  scratchBufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  scratchBufferDeviceAddressInfo.buffer = scratchBuffer;

  VkDeviceAddress scratchBufferAddress = pvkGetBufferDeviceAddressKHR(logicalDevice, &scratchBufferDeviceAddressInfo);

  VkDeviceOrHostAddressKHR scratchDeviceOrHostAddress = {};
  scratchDeviceOrHostAddress.deviceAddress = scratchBufferAddress;

  accelerationStructureBuildGeometryInfo.scratchData = scratchDeviceOrHostAddress;

  VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
    .pNext = NULL,
    .createFlags = 0,
    .buffer = this->accelerationStructureBuffer,
    .offset = 0,
    .size = accelerationStructureBuildSizesInfo.accelerationStructureSize,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
    .deviceAddress = VK_NULL_HANDLE
  };

  pvkCreateAccelerationStructureKHR(logicalDevice, &accelerationStructureCreateInfo, NULL, &this->accelerationStructure);

  accelerationStructureBuildGeometryInfo.dstAccelerationStructure = this->accelerationStructure;

  VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo = {
    .primitiveCount = (uint32_t)bottomLevelAccelerationStructureInstanceList.size(),
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

TopLevelAccelerationStructure::~TopLevelAccelerationStructure() {

}

VkAccelerationStructureKHR* TopLevelAccelerationStructure::getAccelerationStructurePointer() {
  return &this->accelerationStructure;
}