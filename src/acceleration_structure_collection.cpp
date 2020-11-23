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

    }
  }
}
  
AccelerationStructureCollection::~AccelerationStructureCollection() {

}