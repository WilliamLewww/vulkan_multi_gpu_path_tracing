#include "device_manager.h"

DeviceManager::DeviceManager(VkInstance vulkanInstance) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

  std::vector<VkPhysicalDevice> physicalDeviceList(deviceCount);
  vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, physicalDeviceList.data());

  for (int x = 0; x < deviceCount; x++) {
    if (x == 0) {
      this->deviceList.push_back(new DisplayDevice(physicalDeviceList[x]));
      this->displayDevice = (DisplayDevice*)this->deviceList[0];
    }
    else {
      this->deviceList.push_back(new Device(physicalDeviceList[x]));
    }
  }
}

DeviceManager::~DeviceManager() {

}

int DeviceManager::getDeviceCount() {
  return this->deviceList.size();
}

Device* DeviceManager::getDeviceAddressFromIndex(int x) {
  return this->deviceList[x];
}

DisplayDevice* DeviceManager::getDisplayDeviceAddress() {
  return this->displayDevice;
}