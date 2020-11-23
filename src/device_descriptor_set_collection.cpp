#include "device_descriptor_set_collection.h"

DeviceDescriptorSetCollection::DeviceDescriptorSetCollection(std::vector<std::vector<DeviceDescriptor*>> separatedDeviceDescriptorList) {
  for (int x = 0; x < separatedDeviceDescriptorList.size(); x++) {
    this->deviceDescriptorSetList.push_back(new DeviceDescriptorSet(separatedDeviceDescriptorList[x]));
    
    for (int y = 0; y < separatedDeviceDescriptorList[x].size(); y++) {
      bool typeExists = false;
      for (int z = 0; z < this->descriptorPoolSizeList.size(); z++) {
        if (this->descriptorPoolSizeList[z].type == separatedDeviceDescriptorList[x][y]->getDescriptorType()) {
          this->descriptorPoolSizeList[z].descriptorCount += 1;
          typeExists = true;
        }
      }

      if (!typeExists) {
        VkDescriptorPoolSize descriptorPoolSize = {
          .type = separatedDeviceDescriptorList[x][y]->getDescriptorType(),
          .descriptorCount = 1
        };
        this->descriptorPoolSizeList.push_back(descriptorPoolSize);
      }
    }
  }
}

DeviceDescriptorSetCollection::~DeviceDescriptorSetCollection() {

}