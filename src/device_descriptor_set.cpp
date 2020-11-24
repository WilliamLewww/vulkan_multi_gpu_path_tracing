#include "device_descriptor_set.h"

DeviceDescriptorSet::DeviceDescriptorSet(std::vector<DeviceDescriptor*> deviceDescriptorList) {
  for (int x = 0; x < deviceDescriptorList.size(); x++) {
    this->deviceDescriptorList.push_back(deviceDescriptorList[x]);
    this->deviceDescriptorList.back()->setDestinationDescriptorSet(this->descriptorSet);
  }
}

DeviceDescriptorSet::~DeviceDescriptorSet() {

}

VkDescriptorSet& DeviceDescriptorSet::getDescriptorSet() {
  return this->descriptorSet;
}

VkDescriptorSetLayout& DeviceDescriptorSet::getDescriptorSetLayout() {
  return this->descriptorSetLayout;
}

std::vector<VkDescriptorSetLayoutBinding> DeviceDescriptorSet::getDescriptorSetLayoutBindingList() {
  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindingList;

  for (int x = 0; x < this->deviceDescriptorList.size(); x++) {
    descriptorSetLayoutBindingList.push_back(this->deviceDescriptorList[x]->getDescriptorSetLayoutBinding());
  }

  return descriptorSetLayoutBindingList;
}

std::vector<VkWriteDescriptorSet> DeviceDescriptorSet::getWriteDescriptorSetList() {
  std::vector<VkWriteDescriptorSet> writeDescriptorList;

  for (int x = 0; x < this->deviceDescriptorList.size(); x++) {
    writeDescriptorList.push_back(this->deviceDescriptorList[x]->getWriteDescriptorSet());
  }

  return writeDescriptorList;
}