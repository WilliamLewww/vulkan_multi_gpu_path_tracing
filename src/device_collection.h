#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "device.h"

class DeviceCollection {
private:
  std::vector<Device*> deviceList;
public:
  DeviceCollection(VkInstance vulkanInstance);
  ~DeviceCollection();
};