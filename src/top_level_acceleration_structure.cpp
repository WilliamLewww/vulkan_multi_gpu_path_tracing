#include "top_level_acceleration_structure.h"

TopLevelAccelerationStructure::TopLevelAccelerationStructure(std::vector<VkAccelerationStructureInstanceKHR> bottomLevelAccelerationStructureInstanceList,
                                                             VkDevice logicalDevice, 
                                                             VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                             VkCommandPool commandPool,
                                                             VkQueue queue) {

  // PFN_vkCreateAccelerationStructureKHR pvkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(logicalDevice, "vkCreateAccelerationStructureKHR");
  // PFN_vkGetAccelerationStructureMemoryRequirementsKHR pvkGetAccelerationStructureMemoryRequirementsKHR = (PFN_vkGetAccelerationStructureMemoryRequirementsKHR)vkGetDeviceProcAddr(logicalDevice, "vkGetAccelerationStructureMemoryRequirementsKHR");
  // PFN_vkBindAccelerationStructureMemoryKHR pvkBindAccelerationStructureMemoryKHR = (PFN_vkBindAccelerationStructureMemoryKHR)vkGetDeviceProcAddr(logicalDevice, "vkBindAccelerationStructureMemoryKHR");
  // PFN_vkCmdBuildAccelerationStructureKHR pvkCmdBuildAccelerationStructureKHR = (PFN_vkCmdBuildAccelerationStructureKHR)vkGetDeviceProcAddr(logicalDevice, "vkCmdBuildAccelerationStructureKHR");
  // PFN_vkGetBufferDeviceAddressKHR pvkGetBufferDeviceAddressKHR = (PFN_vkGetBufferDeviceAddressKHR)vkGetDeviceProcAddr(logicalDevice, "vkGetBufferDeviceAddressKHR");
  // PFN_vkGetAccelerationStructureDeviceAddressKHR pvkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(logicalDevice, "vkGetAccelerationStructureDeviceAddressKHR");

  // VkAccelerationStructureCreateGeometryTypeInfoKHR geometryInfos = {
  //   .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR,
  //   .pNext = NULL,
  //   .geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR,
  //   .maxPrimitiveCount = (uint32_t)bottomLevelAccelerationStructureInstanceList.size(),
  //   .indexType = VK_INDEX_TYPE_NONE_NV,
  //   .maxVertexCount = 0,
  //   .vertexFormat = VK_FORMAT_UNDEFINED,
  //   .allowsTransforms = VK_TRUE
  // };

  // VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo = {
  //   .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
  //   .pNext = NULL,
  //   .compactedSize = 0,
  //   .type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
  //   .flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
  //   .maxGeometryCount = 1,
  //   .pGeometryInfos = &geometryInfos,
  //   .deviceAddress = VK_NULL_HANDLE
  // };

  // if (pvkCreateAccelerationStructureKHR(logicalDevice, &accelerationStructureCreateInfo, NULL, &this->accelerationStructure) != VK_SUCCESS) {
  //   printf("%s\n", "failed to create acceleration structure");
  // }

  // // ==============================================================================================================

  // VkAccelerationStructureMemoryRequirementsInfoKHR memoryRequirementsInfo = {
  //   .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR,
  //   .pNext = NULL,
  //   .type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_KHR,
  //   .buildType = VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
  //   .accelerationStructure = this->accelerationStructure
  // };

  // VkMemoryRequirements2 memoryRequirements = {
  //   .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
  //   .pNext = NULL
  // };
  
  // pvkGetAccelerationStructureMemoryRequirementsKHR(logicalDevice, &memoryRequirementsInfo, &memoryRequirements);

  // VkDeviceSize accelerationStructureSize = memoryRequirements.memoryRequirements.size;

  // BufferFactory::createBuffer(logicalDevice,
  //                             physicalDeviceMemoryProperties, 
  //                             accelerationStructureSize, 
  //                             VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
  //                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
  //                             &this->accelerationStructureBuffer, 
  //                             &this->accelerationStructureBufferMemory);

  // const VkBindAccelerationStructureMemoryInfoKHR accelerationStructureMemoryInfo = {
  //   .sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_KHR,
  //   .pNext = NULL,
  //   .accelerationStructure = this->accelerationStructure,
  //   .memory = this->accelerationStructureBufferMemory,
  //   .memoryOffset = 0,
  //   .deviceIndexCount = 0,
  //   .pDeviceIndices = NULL
  // };

  // pvkBindAccelerationStructureMemoryKHR(logicalDevice, 1, &accelerationStructureMemoryInfo);

  // // ==============================================================================================================

  // VkDeviceSize geometryInstanceBufferSize = bottomLevelAccelerationStructureInstanceList.size() * sizeof(VkAccelerationStructureInstanceKHR);
  
  // VkBuffer geometryInstanceStagingBuffer;
  // VkDeviceMemory geometryInstanceStagingBufferMemory;
  // BufferFactory::createBuffer(logicalDevice,
  //                             physicalDeviceMemoryProperties,
  //                             geometryInstanceBufferSize, 
  //                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
  //                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
  //                             &geometryInstanceStagingBuffer, 
  //                             &geometryInstanceStagingBufferMemory);

  // void* geometryInstanceData;
  // vkMapMemory(logicalDevice, geometryInstanceStagingBufferMemory, 0, geometryInstanceBufferSize, 0, &geometryInstanceData);
  // memcpy(geometryInstanceData, bottomLevelAccelerationStructureInstanceList.data(), geometryInstanceBufferSize);
  // vkUnmapMemory(logicalDevice, geometryInstanceStagingBufferMemory);

  // VkBuffer geometryInstanceBuffer;
  // VkDeviceMemory geometryInstanceBufferMemory;
  // BufferFactory::createBuffer(logicalDevice, 
  //                             physicalDeviceMemoryProperties, 
  //                             geometryInstanceBufferSize, 
  //                             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
  //                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
  //                             &geometryInstanceBuffer, 
  //                             &geometryInstanceBufferMemory);  

  // BufferFactory::copyBuffer(logicalDevice, 
  //                           commandPool, 
  //                           queue, 
  //                           geometryInstanceStagingBuffer, 
  //                           geometryInstanceBuffer, 
  //                           geometryInstanceBufferSize);

  // vkDestroyBuffer(logicalDevice, geometryInstanceStagingBuffer, NULL);
  // vkFreeMemory(logicalDevice, geometryInstanceStagingBufferMemory, NULL);

  // VkBufferDeviceAddressInfo geometryInstanceBufferDeviceAddressInfo = {
  //   .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
  //   .pNext = NULL,
  //   .buffer = geometryInstanceBuffer
  // };

  // VkDeviceAddress geometryInstanceBufferAddress = pvkGetBufferDeviceAddressKHR(logicalDevice, &geometryInstanceBufferDeviceAddressInfo);

  // VkDeviceOrHostAddressConstKHR geometryInstanceDeviceOrHostAddressConst = {
  //   .deviceAddress = geometryInstanceBufferAddress
  // };

  // VkAccelerationStructureGeometryInstancesDataKHR instancesData = {
  //   .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
  //   .pNext = NULL,
  //   .arrayOfPointers = VK_FALSE,
  //   .data = geometryInstanceDeviceOrHostAddressConst 
  // };

  // VkAccelerationStructureGeometryDataKHR geometryData = {
  //   .instances = instancesData
  // };

  // VkAccelerationStructureGeometryKHR geometry = {
  //   .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
  //   .pNext = NULL,
  //   .geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR,
  //   .geometry = geometryData,
  //   .flags = 0
  // };

  // std::vector<VkAccelerationStructureGeometryKHR> geometries = {geometry};
  // VkAccelerationStructureGeometryKHR* geometriesPointer = geometries.data();

  // VkBuffer scratchBuffer;
  // VkDeviceMemory scratchBufferMemory;
  // BufferFactory::createBuffer(logicalDevice, 
  //                             physicalDeviceMemoryProperties, 
  //                             accelerationStructureSize, 
  //                             VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
  //                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
  //                             &scratchBuffer, 
  //                             &scratchBufferMemory);

  // VkBufferDeviceAddressInfo scratchBufferDeviceAddressInfo = {
  //   .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
  //   .pNext = NULL,
  //   .buffer = scratchBuffer
  // };

  // VkDeviceAddress scratchBufferAddress = pvkGetBufferDeviceAddressKHR(logicalDevice, &scratchBufferDeviceAddressInfo);

  // VkDeviceOrHostAddressKHR scratchDeviceOrHostAddress = {
  //   .deviceAddress = scratchBufferAddress
  // };

  // VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo = {
  //   .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
  //   .pNext = NULL,
  //   .type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
  //   .flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
  //   .update = VK_FALSE,
  //   .srcAccelerationStructure = VK_NULL_HANDLE,
  //   .dstAccelerationStructure = this->accelerationStructure,
  //   .geometryArrayOfPointers = VK_FALSE,
  //   .geometryCount = (uint32_t)geometries.size(),
  //   .ppGeometries = &geometriesPointer,
  //   .scratchData = scratchDeviceOrHostAddress
  // };

  // VkAccelerationStructureBuildOffsetInfoKHR buildOffsetInfo = {
  //   .primitiveCount = (uint32_t)bottomLevelAccelerationStructureInstanceList.size(),
  //   .primitiveOffset = 0,
  //   .firstVertex = 0,
  //   .transformOffset = 0  
  // };
  // std::vector<VkAccelerationStructureBuildOffsetInfoKHR*> buildOffsetInfos = { &buildOffsetInfo };

  // VkCommandBufferAllocateInfo bufferAllocateInfo = {
  //   .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
  //   .pNext = NULL,
  //   .commandPool = commandPool,
  //   .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
  //   .commandBufferCount = 1
  // };

  // VkCommandBuffer commandBuffer;
  // vkAllocateCommandBuffers(logicalDevice, &bufferAllocateInfo, &commandBuffer);
  
  // VkCommandBufferBeginInfo commandBufferBeginInfo = {
  //   .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
  //   .pNext = NULL,
  //   .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  //   .pInheritanceInfo = NULL
  // };

  // vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
  // pvkCmdBuildAccelerationStructureKHR(commandBuffer, 1, &buildGeometryInfo, buildOffsetInfos.data());
  // vkEndCommandBuffer(commandBuffer);

  // VkSubmitInfo submitInfo = {
  //   .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
  //   .pNext = NULL,
  //   .waitSemaphoreCount = 0,
  //   .pWaitSemaphores = NULL,
  //   .pWaitDstStageMask = NULL,
  //   .commandBufferCount = 1,
  //   .pCommandBuffers = &commandBuffer,
  //   .signalSemaphoreCount = 0,
  //   .pSignalSemaphores = NULL
  // };

  // vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
  // vkQueueWaitIdle(queue);

  // vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);

  // vkDestroyBuffer(logicalDevice, scratchBuffer, NULL);
  // vkFreeMemory(logicalDevice, scratchBufferMemory, NULL);
}

TopLevelAccelerationStructure::~TopLevelAccelerationStructure() {

}

VkAccelerationStructureKHR* TopLevelAccelerationStructure::getAccelerationStructurePointer() {
  return &this->accelerationStructure;
}