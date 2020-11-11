#include "renderer.h"

Renderer::Renderer(Model* model, Camera* camera) {
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

  displayDevice->createVertexBuffer(model);
  displayDevice->createIndexBuffer(model);
  displayDevice->createMaterialBuffers(model);
  displayDevice->createTextures();
  displayDevice->createUniformBuffers();

  this->accelerationStructureManager = new AccelerationStructureManager();
  
  this->accelerationStructureManager->initializeContainerOnDevice(displayDevice);

  this->accelerationStructureManager->createBottomLevelAccelerationStructure(displayDevice, 
                                                                             model->getPrimitiveCount(), 
                                                                             model->getVertexCount(), 
                                                                             displayDevice->getVertexBuffer(), 
                                                                             displayDevice->getIndexBuffer());

  VkTransformMatrixKHR transformMatrix = {
    .matrix = {
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0}
    }
  };
  this->accelerationStructureManager->addBottomLevelAccelerationStructureInstance(displayDevice, 0, 0, transformMatrix);

  this->accelerationStructureManager->createTopLevelAccelerationStructure(displayDevice);

  this->descriptorManager = new DescriptorManager();

  this->descriptorManager->initializeContainerOnDevice(displayDevice, 2);

  VkWriteDescriptorSetAccelerationStructureKHR descriptorSetAccelerationStructure = {
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR,
    .pNext = NULL,
    .accelerationStructureCount = 1,
    .pAccelerationStructures = this->accelerationStructureManager->getTopLevelAccelerationStructurePointer(displayDevice)
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         0, 
                                         0, 
                                         VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         NULL, 
                                         NULL,
                                         NULL,
                                         &descriptorSetAccelerationStructure);

  VkDescriptorBufferInfo cameraUniformBufferInfo = {
    .buffer = displayDevice->getCameraUniformBuffer(),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         0, 
                                         1, 
                                         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
                                         (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
                                         NULL, 
                                         &cameraUniformBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo indexBufferInfo = {
    .buffer = displayDevice->getIndexBuffer(),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         0, 
                                         2, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         NULL, 
                                         &indexBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo vertexBufferInfo = {
    .buffer = displayDevice->getVertexBuffer(),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         0, 
                                         3, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         NULL, 
                                         &vertexBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorImageInfo imageInfo = {
    .imageView = displayDevice->getRayTraceImageView(),
    .imageLayout = VK_IMAGE_LAYOUT_GENERAL
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         0, 
                                         4, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         &imageInfo, 
                                         NULL,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo transformUniformBufferInfo = {
    .buffer = displayDevice->getTransformUniformBuffer(),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         0, 
                                         5, 
                                         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
                                         (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
                                         NULL, 
                                         &transformUniformBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo materialIndexBufferInfo = {
    .buffer = displayDevice->getMaterialIndexBuffer(),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         1, 
                                         0, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
                                         NULL, 
                                         &materialIndexBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo materialBufferInfo = {
    .buffer = displayDevice->getMaterialBuffer(),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         1, 
                                         1, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
                                         NULL, 
                                         &materialBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo materialLightBufferInfo = {
    .buffer = displayDevice->getMaterialLightBuffer(),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         1, 
                                         2, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
                                         NULL, 
                                         &materialLightBufferInfo,
                                         NULL,
                                         NULL);

  this->descriptorManager->concludeDescriptors(displayDevice);

  this->graphicsPipeline = new GraphicsPipeline();
  this->graphicsPipeline->setVertexFile("bin/basic.vert.spv");
  this->graphicsPipeline->setFragmentFile("bin/basic.frag.spv");

  this->graphicsPipeline->initializeContainerOnDevice(displayDevice);
  this->graphicsPipeline->createPipelineLayout(displayDevice, descriptorManager->getDescriptorSetLayoutList(displayDevice));

  std::vector<VkVertexInputBindingDescription> vertexBindingDescriptionList(1);
  vertexBindingDescriptionList[0].binding = 0;
  vertexBindingDescriptionList[0].stride = sizeof(float) * 3;
  vertexBindingDescriptionList[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptionList(1);
  vertexAttributeDescriptionList[0].binding = 0;
  vertexAttributeDescriptionList[0].location = 0;
  vertexAttributeDescriptionList[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertexAttributeDescriptionList[0].offset = 0;

  this->graphicsPipeline->createPipeline(displayDevice,
                                         vertexBindingDescriptionList,
                                         vertexAttributeDescriptionList,
                                         displayDevice->getSwapchainExtent(),
                                         displayDevice->getRenderPass());

  displayDevice->createCommandBuffers(model, this->graphicsPipeline->getPipeline(displayDevice), this->graphicsPipeline->getPipelineLayout(displayDevice), this->descriptorManager->getDescriptorSetListReference(displayDevice));
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
  delete this->accelerationStructureManager;
  delete this->deviceManager;
  delete this->vulkanInstance;
  delete this->window;
}