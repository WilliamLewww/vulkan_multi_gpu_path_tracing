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