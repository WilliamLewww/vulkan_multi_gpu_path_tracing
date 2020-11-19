#include "model_collection.h"

ModelCollection::ModelCollection(std::vector<std::string> fileNameList) {
  for (std::string fileName : fileNameList) {
    this->modelList.push_back(new Model(fileName));
  }
}

ModelCollection::~ModelCollection() {
  for (int x = 0; x < this->modelList.size(); x++) {
    delete this->modelList[x];
  }
}

Model* ModelCollection::getModel(int index) {
  return this->modelList[index];
}