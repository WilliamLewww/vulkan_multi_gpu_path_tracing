#include "device_manager.h"

DeviceManager::DeviceManager(VkInstance vulkanInstance) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

  std::vector<VkPhysicalDevice> physicalDeviceList(deviceCount);
  vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, physicalDeviceList.data());

  for (int x = 0; x < deviceCount; x++) {
    this->deviceList.push_back(Device(physicalDeviceList[x]));
  }
}

DeviceManager::~DeviceManager() {

}

int DeviceManager::getDeviceCount() {
  return this->deviceList.size();
}

Device* DeviceManager::getDevicePointerFromIndex(int x) {
  return &this->deviceList[x];
}