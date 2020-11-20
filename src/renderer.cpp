#include "renderer.h"

Renderer::Renderer(VkInstance vulkanInstance, VkSurfaceKHR surface, ModelCollection* modelCollection) {
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
  displayDevice->createCommandPool();
  displayDevice->createSwapchain(surface);
  displayDevice->createRenderPass();
  displayDevice->createDepthResource();
  displayDevice->createFramebuffers();
  displayDevice->createTextures();

  std::map<Model*, uint32_t> modelFrequencyMap = {
    {modelCollection->getModel(0), 2},
    {modelCollection->getModel(1), 1}
  };
  displayDevice->createModelInstances(modelFrequencyMap);
}

Renderer::~Renderer() {
  delete this->deviceCollection;
}