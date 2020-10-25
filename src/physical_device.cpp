#include "physical_device.h"

PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) {
  this->device = device;

  vkGetPhysicalDeviceProperties(device, &this->deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &this->deviceFeatures);
}

PhysicalDevice::~PhysicalDevice() {

}