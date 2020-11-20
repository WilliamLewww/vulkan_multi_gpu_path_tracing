#include "model_instance_collection.h"

ModelInstanceCollection::ModelInstanceCollection(std::map<Model*, uint32_t> modelFrequencyMap) {
  int modelIndex = 0;
  int instanceIndex = 0;
  for (std::pair<Model*, uint32_t> pair : modelFrequencyMap) {
    for (int x = 0; x < pair.second; x++) {
      this->modelInstanceList.push_back(new ModelInstance(pair.first, modelIndex, instanceIndex));
    }
    modelIndex += 1;
  }
}

ModelInstanceCollection::~ModelInstanceCollection() {
  for (int x = 0; x < this->modelInstanceList.size(); x++) {
    delete this->modelInstanceList[x];
  }
}