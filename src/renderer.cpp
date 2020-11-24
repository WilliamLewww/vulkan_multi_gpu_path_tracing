#include "renderer.h"

Renderer::Renderer(VkInstance vulkanInstance, VkSurfaceKHR surface, ModelCollection* modelCollection, Camera* camera, GLFWwindow* window) {
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

  std::map<void*, uint32_t> bufferMap = {
    {camera->getUniformPointer(), camera->getUniformStructureSize()},
    {displayDevice->getModelInstanceCollectionPointer()->getUniformBufferPointer(), displayDevice->getModelInstanceCollectionPointer()->getUniformBufferSize()}
  };
  displayDevice->createUniformBufferCollection(bufferMap);

  displayDevice->createAccelerationStructureCollection(displayDevice->getModelInstanceCollectionPointer()->getModelInstanceMap());

  std::vector<std::vector<DeviceDescriptor*>> separatedDeviceDescriptorList = {
    {
      new DeviceDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, displayDevice->getDeviceUniformBufferCollection()->getDescriptorBufferInfoPointer(0), NULL, NULL),
      new DeviceDescriptor(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, displayDevice->getDeviceUniformBufferCollection()->getDescriptorBufferInfoPointer(1), NULL, NULL),
      new DeviceDescriptor(2, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, NULL, NULL, displayDevice->getAccelerationStructureCollection()->getWriteDescriptorSetAccelerationStructurePointer()),
      new DeviceDescriptor(3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), displayDevice->getDeviceTextures()->getDescriptorRayTraceImageInfoPointer(), NULL, NULL, NULL),
    },
    {
      new DeviceDescriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, displayDevice->getModelInstanceCollection()->getDescriptorTotalIndexBufferInfoPointer(), NULL, NULL),
      new DeviceDescriptor(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, displayDevice->getModelInstanceCollection()->getDescriptorTotalVertexBufferInfoPointer(), NULL, NULL),
      new DeviceDescriptor(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, displayDevice->getModelInstanceCollection()->getDescriptorTotalMaterialIndexBufferInfoPointer(), NULL, NULL),
      new DeviceDescriptor(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, displayDevice->getModelInstanceCollection()->getDescriptorTotalMaterialBufferInfoPointer(), NULL, NULL),
      new DeviceDescriptor(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, displayDevice->getModelInstanceCollection()->getDescriptorTotalMaterialLightBufferInfoPointer(), NULL, NULL),
    }
  };
  displayDevice->createDescriptorSetCollection(separatedDeviceDescriptorList);

  displayDevice->createGraphicsPipeline("bin/basic.vert.spv", "bin/basic.frag.spv");
  displayDevice->createRenderCommandBuffers();

  displayDevice->createSynchronizationObjects();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    displayDevice->getDeviceUniformBufferCollection()->updateUniformBuffer(displayDevice->getLogicalDevice(), 0, camera->getUniformPointer(), camera->getUniformStructureSize());
    displayDevice->drawFrame();
    camera->update();
  }
}

Renderer::~Renderer() {
  delete this->deviceCollection;
}