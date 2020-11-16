#include "renderer.h"

Renderer::Renderer(std::vector<Model*> modelList, Camera* camera) {
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
  displayDevice->createTextures();

  modelList[0]->initializeOnDevice(displayDevice);
  modelList[1]->initializeOnDevice(displayDevice);

  this->instanceManager = new InstanceManager();
  this->instanceManager->initializeContainerOnDevice(displayDevice);

  this->instanceManager->addInstance(displayDevice, modelList[0], 0, 0);

  displayDevice->createUniformBuffers(this->instanceManager->getInstanceCount(displayDevice), this->instanceManager->getTotalTransformBuffer(displayDevice));

  this->accelerationStructureManager = new AccelerationStructureManager();
  this->accelerationStructureManager->initializeContainerOnDevice(displayDevice);

  this->accelerationStructureManager->createBottomLevelAccelerationStructures(displayDevice, modelList);
  this->accelerationStructureManager->addBottomLevelAccelerationStructureInstances(displayDevice, this->instanceManager->getInstanceList(displayDevice));
  this->accelerationStructureManager->createTopLevelAccelerationStructure(displayDevice);

  this->descriptorManager = new DescriptorManager();
  this->descriptorManager->initializeContainerOnDevice(displayDevice, 2);

  VkDescriptorBufferInfo cameraUniformBufferInfo = {
    .buffer = displayDevice->getCameraUniformBuffer(),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         0, 
                                         0, 
                                         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
                                         (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
                                         NULL, 
                                         &cameraUniformBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo transformUniformBufferInfo = {
    .buffer = displayDevice->getTransformUniformBuffer(),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         0, 
                                         1, 
                                         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
                                         (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
                                         NULL, 
                                         &transformUniformBufferInfo,
                                         NULL,
                                         NULL);

  VkWriteDescriptorSetAccelerationStructureKHR descriptorSetAccelerationStructure = {
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR,
    .pNext = NULL,
    .accelerationStructureCount = 1,
    .pAccelerationStructures = this->accelerationStructureManager->getTopLevelAccelerationStructurePointer(displayDevice)
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         0, 
                                         2, 
                                         VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         NULL, 
                                         NULL,
                                         NULL,
                                         &descriptorSetAccelerationStructure);

  VkDescriptorImageInfo imageInfo = {
    .imageView = displayDevice->getRayTraceImageView(),
    .imageLayout = VK_IMAGE_LAYOUT_GENERAL
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         0, 
                                         3, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         &imageInfo, 
                                         NULL,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo indexBufferInfo = {
    .buffer = modelList[0]->getIndexBuffer(displayDevice),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         1, 
                                         0, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         NULL, 
                                         &indexBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo vertexBufferInfo = {
    .buffer = modelList[0]->getVertexBuffer(displayDevice),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         1, 
                                         1, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         VK_SHADER_STAGE_FRAGMENT_BIT,
                                         NULL, 
                                         &vertexBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo materialIndexBufferInfo = {
    .buffer = modelList[0]->getMaterialIndexBuffer(displayDevice),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         1, 
                                         2, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
                                         NULL, 
                                         &materialIndexBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo materialBufferInfo = {
    .buffer = modelList[0]->getMaterialBuffer(displayDevice),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         1, 
                                         3, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
                                         NULL, 
                                         &materialBufferInfo,
                                         NULL,
                                         NULL);

  VkDescriptorBufferInfo materialLightBufferInfo = {
    .buffer = modelList[0]->getMaterialLightBuffer(displayDevice),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
  this->descriptorManager->addDescriptor(displayDevice,
                                         1, 
                                         4, 
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
                                         (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
                                         NULL, 
                                         &materialLightBufferInfo,
                                         NULL,
                                         NULL);

  this->descriptorManager->concludeDescriptors(displayDevice);
  this->descriptorManager->print();

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

  this->commandBufferManager = new CommandBufferManager();
  this->commandBufferManager->initializeContainerOnDevice(displayDevice);

  this->commandBufferManager->createCommandBuffers(displayDevice, 
                                                   instanceManager->getInstanceList(displayDevice), 
                                                   this->graphicsPipeline->getPipeline(displayDevice), 
                                                   this->graphicsPipeline->getPipelineLayout(displayDevice), 
                                                   this->descriptorManager->getDescriptorSetListReference(displayDevice));

  displayDevice->createSynchronizationObjects();

  while (!glfwWindowShouldClose(this->window->getWindow())) {
    glfwPollEvents();
    displayDevice->drawFrame(camera->getUniform(), this->commandBufferManager->getCommandBufferList(displayDevice));
    camera->update();
  }
}

Renderer::~Renderer() {
  delete this->commandBufferManager;
  delete this->graphicsPipeline;
  delete this->descriptorManager;
  delete this->accelerationStructureManager;
  delete this->instanceManager;
  delete this->deviceManager;
  delete this->vulkanInstance;
  delete this->window;
}