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

  this->descriptorManager = new DescriptorManager(2);

  VkWriteDescriptorSetAccelerationStructureKHR descriptorSetAccelerationStructure = {};
  descriptorSetAccelerationStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
  descriptorSetAccelerationStructure.pNext = NULL;
  descriptorSetAccelerationStructure.accelerationStructureCount = 1;
  descriptorSetAccelerationStructure.pAccelerationStructures = displayDevice->getTopLevelAccelerationStructurePointer();  
  this->descriptorManager->addDescriptor(0, 
                                         0, 
                                         VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         NULL, 
                                         NULL,
                                         NULL,
                                         &descriptorSetAccelerationStructure);

  VkDescriptorBufferInfo uniformBufferInfo = {};
  uniformBufferInfo.buffer = displayDevice->getUniformBuffer();
  uniformBufferInfo.offset = 0;
  uniformBufferInfo.range = VK_WHOLE_SIZE;
  this->descriptorManager->addDescriptor(0, 
                                         1, 
                                         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
                                         (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
                                         NULL, 
                                         &uniformBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo indexBufferInfo = {};
  indexBufferInfo.buffer = displayDevice->getIndexBuffer();
  indexBufferInfo.offset = 0;
  indexBufferInfo.range = VK_WHOLE_SIZE;
  this->descriptorManager->addDescriptor(0, 
                                         2, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         NULL, 
                                         &indexBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo vertexBufferInfo = {};
  vertexBufferInfo.buffer = displayDevice->getVertexBuffer();
  vertexBufferInfo.offset = 0;
  vertexBufferInfo.range = VK_WHOLE_SIZE;
  this->descriptorManager->addDescriptor(0, 
                                         3, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         NULL, 
                                         &vertexBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorImageInfo imageInfo = {};
  imageInfo.imageView = displayDevice->getRayTraceImageView();
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
  this->descriptorManager->addDescriptor(0, 
                                         4, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         &imageInfo, 
                                         NULL,
                                         NULL,
                                         NULL);

  this->descriptorManager->concludeDescriptors(displayDevice->getLogicalDevice());

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

  displayDevice->createCommandBuffers(scene, this->graphicsPipeline->getPipeline(), this->graphicsPipeline->getPipelineLayout());
  displayDevice->createSynchronizationObjects();

  while (!glfwWindowShouldClose(this->window->getWindow())) {
    glfwPollEvents();
    displayDevice->drawFrame(camera->getUniform());
    camera->update();
  }
}

Renderer::~Renderer() {
  delete this->graphicsPipeline;
  delete this->descriptorManager;
  delete this->deviceManager;
  delete this->vulkanInstance;
  delete this->window;
}