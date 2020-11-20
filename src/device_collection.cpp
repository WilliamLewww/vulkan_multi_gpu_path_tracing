#include "device_collection.h"

DeviceCollection::DeviceCollection(VkInstance vulkanInstance) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

  std::vector<VkPhysicalDevice> physicalDeviceList(deviceCount);
  vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, physicalDeviceList.data());

  for (int x = 0; x < deviceCount; x++) {
    this->deviceList.push_back(new Device(physicalDeviceList[x]));
  }
}

DeviceCollection::~DeviceCollection() {
  for (int x = 0; x < this->deviceList.size(); x++) {
    delete this->deviceList[x];
  }
}

Device* DeviceCollection::getDevice(int index) {
  return this->deviceList[index];
}