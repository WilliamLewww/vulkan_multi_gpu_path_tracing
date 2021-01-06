#include "acceleration_structure_collection.h"

AccelerationStructureCollection::AccelerationStructureCollection(std::vector<std::map<Model*, std::vector<ModelInstance*>>> modelInstanceMapList, 
                                                                 VkDevice logicalDevice, 
                                                                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                                 VkCommandPool commandPool,
                                                                 VkQueue queue) {

  for (int x = 0; x < modelInstanceMapList.size(); x++) {
    this->accelerationStructureSetList.push_back(new AccelerationStructureSet(modelInstanceMapList[x], 
                                                                              logicalDevice, 
                                                                              physicalDeviceMemoryProperties, 
                                                                              commandPool, 
                                                                              queue));
  }

}

AccelerationStructureCollection::~AccelerationStructureCollection() {

}

AccelerationStructureSet* AccelerationStructureCollection::getAccelerationStructureSet(int index) {
  return this->accelerationStructureSetList[index];
}