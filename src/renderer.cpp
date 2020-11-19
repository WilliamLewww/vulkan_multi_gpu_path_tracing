#include "renderer.h"

Renderer::Renderer(VkInstance vulkanInstance, VkSurfaceKHR surface) {
  this->deviceCollection = new DeviceCollection(vulkanInstance);

  Device* displayDevice = this->deviceCollection->getDevice(0);
  displayDevice->initializeDeviceQueue(surface);

  std::vector<const char*> deviceExtensionList {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME, 
    "VK_KHR_ray_tracing", 
    "VK_KHR_get_memory_requirements2",
    "VK_EXT_descriptor_indexing",
    "VK_KHR_buffer_device_address",
    "VK_KHR_deferred_host_operations",
    "VK_KHR_pipeline_library",
    "VK_KHR_maintenance3",
    "VK_KHR_maintenance1"
  };
  displayDevice->createLogicalDevice(deviceExtensionList);
  displayDevice->createDeviceCommandPool();
  displayDevice->createDeviceSwapchain(surface);
}

Renderer::~Renderer() {
  delete this->deviceCollection;
}