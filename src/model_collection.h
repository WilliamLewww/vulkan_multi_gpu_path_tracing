#pragma once

#include <vector>
#include <string>

#include "model.h"

class ModelCollection {
private:
  std::vector<Model*> modelList;
public:
  ModelCollection(std::vector<std::string> fileNameList);
  ~ModelCollection();
};