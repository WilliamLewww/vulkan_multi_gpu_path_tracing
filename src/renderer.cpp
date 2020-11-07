#include "renderer.h"

Renderer::Renderer(Scene* scene, Camera* camera) {
  this->window = new Window(800, 600);
  this->window->setKeyCallback(Input::keyCallback);
  this->window->setCursorPositionCallback(Input::cursorPositionCallback);

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

  this->graphicsPipeline = new GraphicsPipeline();
  this->graphicsPipeline->setVertexFile("bin/basic.vert.spv");
  this->graphicsPipeline->setFragmentFile("bin/basic.frag.spv");
  this->graphicsPipeline->createPipelineLayout(displayDevice->getLogicalDevice(), displayDevice->getRayTraceDescriptorSetLayoutList());

  std::vector<VkVertexInputBindingDescription> vertexBindingDescriptionList(1);
  vertexBindingDescriptionList[0].binding = 0;
  vertexBindingDescriptionList[0].stride = sizeof(float) * 3;
  vertexBindingDescriptionList[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptionList(1);
  vertexAttributeDescriptionList[0].binding = 0;
  vertexAttributeDescriptionList[0].location = 0;
  vertexAttributeDescriptionList[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertexAttributeDescriptionList[0].offset = 0;

  this->graphicsPipeline->createGraphicsPipeline(displayDevice->getLogicalDevice(),
                              vertexBindingDescriptionList,
                              vertexAttributeDescriptionList,
                              displayDevice->getSwapchainExtent(),
                              displayDevice->getRenderPass());

  displayDevice->createCommandBuffers(scene, this->graphicsPipeline->getPipelineLayout(), this->graphicsPipeline->getPipeline());
  displayDevice->createSynchronizationObjects();

  while (!glfwWindowShouldClose(this->window->getWindow())) {
    glfwPollEvents();
    displayDevice->drawFrame(camera->getUniform());
    camera->update();
  }
}

Renderer::~Renderer() {
  delete this->window;
  delete this->graphicsPipeline;
}