#include "device_descriptor_set.h"

DeviceDescriptorSet::DeviceDescriptorSet(std::vector<DeviceDescriptor*> deviceDescriptorList) {
  for (int x = 0; x < deviceDescriptorList.size(); x++) {
    this->deviceDescriptorList.push_back(deviceDescriptorList[x]);
    this->deviceDescriptorList.back()->setDestinationDescriptorSet(this->descriptorSet);
  }
}

DeviceDescriptorSet::~DeviceDescriptorSet() {

}