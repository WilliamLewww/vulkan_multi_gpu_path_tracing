#include "model_instance_set_collection.h"

ModelInstanceSetCollection::ModelInstanceSetCollection(std::vector<std::map<Model*, std::vector<TRS>>> modelFrequencyMapList,
                                                 VkDevice logicalDevice, 
                                                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                                                 VkCommandPool commandPool,
                                                 VkQueue queue) {

  for (int x = 0; x < modelFrequencyMapList.size(); x++) {
    this->modelInstanceSetList.push_back(new ModelInstanceSet(modelFrequencyMapList[x],
                                                              logicalDevice, 
                                                              physicalDeviceMemoryProperties, 
                                                              commandPool,
                                                              queue));
  }
}

ModelInstanceSetCollection::~ModelInstanceSetCollection() {

}

std::vector<std::map<Model*, std::vector<ModelInstance*>>> ModelInstanceSetCollection::getModelInstanceMapList() {
  std::vector<std::map<Model*, std::vector<ModelInstance*>>> modelInstanceMapList;

  for (int x = 0; x < this->modelInstanceSetList.size(); x++) {
    modelInstanceMapList.push_back(this->modelInstanceSetList[x]->getModelInstanceMap());
  }

  return modelInstanceMapList;
}

uint32_t ModelInstanceSetCollection::getModelInstanceSetCount() {
  return this->modelInstanceSetList.size();
}

ModelInstanceSet* ModelInstanceSetCollection::getModelInstanceSet(int index) {
  return this->modelInstanceSetList[index];
}