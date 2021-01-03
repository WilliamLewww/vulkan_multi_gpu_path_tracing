#include "acceleration_structure_collection.h"

AccelerationStructureCollection::AccelerationStructureCollection(std::map<Model*, std::vector<ModelInstance*>> modelInstanceMap, 
                                                                 VkDevice logicalDevice, 
                                                                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                                 VkCommandPool commandPool,
                                                                 VkQueue queue) {
  
  for (std::pair<Model*, std::vector<ModelInstance*>> modelInstancePair : modelInstanceMap) {
    this->bottomLevelAccelerationStructureList.push_back(new BottomLevelAccelerationStructure(modelInstancePair.first,
                                                                                              modelInstancePair.second[0]->getVertexBuffer(),
                                                                                              modelInstancePair.second[0]->getIndexBuffer(),
                                                                                              logicalDevice,
                                                                                              physicalDeviceMemoryProperties,
                                                                                              commandPool,
                                                                                              queue));

    for (int x = 0; x < modelInstancePair.second.size(); x++) {
      this->addBottomLevelAccelerationStructureInstance(this->bottomLevelAccelerationStructureList.back(), modelInstancePair.second[x], logicalDevice);
    }
  }

  this->topLevelAccelerationStructure = new TopLevelAccelerationStructure(this->bottomLevelAccelerationStructureInstanceList, logicalDevice, physicalDeviceMemoryProperties, commandPool, queue);

  this->writeDescriptorSetAccelerationStructure = {
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR,
    .pNext = NULL,
    .accelerationStructureCount = 1,
    .pAccelerationStructures = this->topLevelAccelerationStructure->getAccelerationStructurePointer()
  };
}
  
AccelerationStructureCollection::~AccelerationStructureCollection() {

}

void AccelerationStructureCollection::addBottomLevelAccelerationStructureInstance(BottomLevelAccelerationStructure* bottomLevelAccelerationStructure, ModelInstance* modelInstance, VkDevice logicalDevice) {
  PFN_vkGetAccelerationStructureDeviceAddressKHR pvkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(logicalDevice, "vkGetAccelerationStructureDeviceAddressKHR");

  VkAccelerationStructureDeviceAddressInfoKHR accelerationStructureDeviceAddressInfo = {
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR,
    .pNext = NULL,
    .accelerationStructure = bottomLevelAccelerationStructure->getAccelerationStructure()
  };

  VkDeviceAddress accelerationStructureDeviceAddress = pvkGetAccelerationStructureDeviceAddressKHR(logicalDevice, &accelerationStructureDeviceAddressInfo);

  VkAccelerationStructureInstanceKHR geometryInstance = {
    .transform = modelInstance->getTransformation().getVulkanTransformMatrix(),
    .instanceCustomIndex = modelInstance->getInstanceIndex(),
    .mask = 0xFF,
    .instanceShaderBindingTableRecordOffset = 0,
    .flags = 0,
    .accelerationStructureReference = accelerationStructureDeviceAddress
  };

  this->bottomLevelAccelerationStructureInstanceList.push_back(geometryInstance);
}

VkWriteDescriptorSetAccelerationStructureKHR* AccelerationStructureCollection::getWriteDescriptorSetAccelerationStructurePointer() {
  return &this->writeDescriptorSetAccelerationStructure;
}

void AccelerationStructureCollection::updateAccelerationStructure(std::vector<ModelInstance*> modelInstanceList,
                                                                  VkDevice logicalDevice, 
                                                                  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                                  VkCommandPool commandPool,
                                                                  VkQueue queue) {
  for (int x = 0; x < modelInstanceList.size(); x++) {
    this->bottomLevelAccelerationStructureInstanceList[x].transform = modelInstanceList[x]->getTransformation().getVulkanTransformMatrix();
  }

  this->topLevelAccelerationStructure->updateAccelerationStructure(this->bottomLevelAccelerationStructureInstanceList, logicalDevice, physicalDeviceMemoryProperties, commandPool, queue);
}