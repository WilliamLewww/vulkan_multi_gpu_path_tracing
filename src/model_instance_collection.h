#pragma once

#include <vector>
#include <map>

#include "model_instance.h"

class ModelInstanceCollection {
private:
  std::vector<ModelInstance*> modelInstanceList;
public:
  ModelInstanceCollection(std::map<Model*, uint32_t> modelFrequencyMap);
  ~ModelInstanceCollection();
};