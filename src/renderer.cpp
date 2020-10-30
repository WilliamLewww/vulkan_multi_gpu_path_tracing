#include "renderer.h"

Renderer::Renderer(Scene* scene) {
  this->window = new Window(800, 600);
  this->window->setKeyCallback(Input::keyCallback);

  std::vector<const char*> instanceExtensionList {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
  std::vector<const char*> instanceLayerList {"VK_LAYER_KHRONOS_validation"};
  this->vulkanInstance = new VulkanInstance("Vulkan Multi-GPU Ray Tracing", instanceExtensionList, instanceLayerList, true);
  this->window->createWindowSurface(this->vulkanInstance->getInstance());

  this->deviceManager = new DeviceManager(this->vulkanInstance->getInstance());

  Device* displayDevice = this->deviceManager->getDisplayDeviceAddress();
  displayDevice->initializeQueues(this->window->getSurface());

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
  displayDevice->createSwapchain(this->window->getSurface());
  displayDevice->createRenderPass();
  displayDevice->createCommandPool();
  displayDevice->createDepthResource();
  displayDevice->createFramebuffers();

  displayDevice->createVertexBuffer(scene);
  displayDevice->createIndexBuffer(scene);
  displayDevice->createMaterialBuffers(scene);
  displayDevice->createTextures();

  displayDevice->createAccelerationStructure(scene);
  displayDevice->bindAccelerationStructure();
  displayDevice->buildAccelerationStructure(scene);
  displayDevice->createTopLevelAccelerationStructure();

  displayDevice->createUniformBuffer();
  displayDevice->createDescriptorSets();
}

Renderer::~Renderer() {
  delete this->window;
}