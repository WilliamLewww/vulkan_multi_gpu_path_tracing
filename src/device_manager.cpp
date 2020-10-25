#include "device_manager.h"

DeviceManager::DeviceManager(VkInstance vulkanInstance) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

  std::vector<VkPhysicalDevice> deviceList(deviceCount);
  vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, deviceList.data());

  for (int x = 0; x < deviceCount; x++) {
    physicalDeviceList.push_back(PhysicalDevice(deviceList[x]));
  }
}

DeviceManager::~DeviceManager() {

}