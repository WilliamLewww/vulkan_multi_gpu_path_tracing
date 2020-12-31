#include "descriptor_set.h"

DescriptorSet::DescriptorSet(std::vector<Descriptor*> descriptorList) {
  for (int x = 0; x < descriptorList.size(); x++) {
    this->descriptorList.push_back(descriptorList[x]);
    this->descriptorList.back()->setDestinationDescriptorSet(this->descriptorSet);
  }
}

DescriptorSet::~DescriptorSet() {

}

VkDescriptorSet& DescriptorSet::getDescriptorSet() {
  return this->descriptorSet;
}

VkDescriptorSetLayout& DescriptorSet::getDescriptorSetLayout() {
  return this->descriptorSetLayout;
}

std::vector<VkDescriptorSetLayoutBinding> DescriptorSet::getDescriptorSetLayoutBindingList() {
  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindingList;

  for (int x = 0; x < this->descriptorList.size(); x++) {
    descriptorSetLayoutBindingList.push_back(this->descriptorList[x]->getDescriptorSetLayoutBinding());
  }

  return descriptorSetLayoutBindingList;
}

std::vector<VkWriteDescriptorSet> DescriptorSet::getWriteDescriptorSetList() {
  std::vector<VkWriteDescriptorSet> writeDescriptorList;

  for (int x = 0; x < this->descriptorList.size(); x++) {
    writeDescriptorList.push_back(this->descriptorList[x]->getWriteDescriptorSet());
  }

  return writeDescriptorList;
}