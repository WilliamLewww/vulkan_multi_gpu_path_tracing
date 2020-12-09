#include "renderer.h"

Renderer::Renderer(VkInstance vulkanInstance, VkSurfaceKHR surface, ModelCollection* modelCollection, Camera* camera) {
  this->deviceCollection = new DeviceCollection(vulkanInstance);

  this->displayDevice = this->deviceCollection->getDevice(0);
  this->displayDevice->initializeDeviceQueue(surface);

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
  this->displayDevice->createLogicalDevice(deviceExtensionList);
  this->displayDevice->createCommandPool();
  this->displayDevice->createSwapchain(surface);
  this->displayDevice->createRenderPass();
  this->displayDevice->createDepthResource();
  this->displayDevice->createFramebuffers();
  this->displayDevice->createTextures();

  std::map<Model*, std::vector<Matrix4x4>> modelFrequencyMap = {
    {
      modelCollection->getModel(7), 
      {
        createTranslateMatrix4x4(0, 0, 0)
      }
    }
  };
  this->displayDevice->createModelInstances(modelFrequencyMap);

  std::map<void*, uint32_t> bufferMap = {
    {camera->getUniformPointer(), camera->getUniformStructureSize()},
    {this->displayDevice->getModelInstanceCollectionPointer()->getUniformBufferPointer(), this->displayDevice->getModelInstanceCollectionPointer()->getUniformBufferSize()}
  };
  this->displayDevice->createUniformBufferCollection(bufferMap);

  this->displayDevice->createAccelerationStructureCollection(this->displayDevice->getModelInstanceCollectionPointer()->getModelInstanceMap());

  std::vector<std::vector<DeviceDescriptor*>> separatedDeviceDescriptorList = {
    {
      new DeviceDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getDeviceUniformBufferCollection()->getDescriptorBufferInfoPointer(0), NULL, NULL),
      new DeviceDescriptor(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getDeviceUniformBufferCollection()->getDescriptorBufferInfoPointer(1), NULL, NULL),
      new DeviceDescriptor(2, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, NULL, NULL, this->displayDevice->getAccelerationStructureCollection()->getWriteDescriptorSetAccelerationStructurePointer()),
      new DeviceDescriptor(3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), this->displayDevice->getDeviceTextures()->getDescriptorRayTraceImageInfoPointer(), NULL, NULL, NULL),
    },
    {
      new DeviceDescriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalIndexBufferInfoPointer(), NULL, NULL),
      new DeviceDescriptor(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalVertexBufferInfoPointer(), NULL, NULL),
      new DeviceDescriptor(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalMaterialIndexBufferInfoPointer(), NULL, NULL),
      new DeviceDescriptor(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalMaterialBufferInfoPointer(), NULL, NULL),
      new DeviceDescriptor(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalMaterialLightBufferInfoPointer(), NULL, NULL),
    }
  };
  this->displayDevice->createDescriptorSetCollection(separatedDeviceDescriptorList);

  this->displayDevice->createGraphicsPipeline("bin/basic.vert.spv", "bin/basic.frag.spv");
  this->displayDevice->createRenderCommandBuffers();

  this->displayDevice->createSynchronizationObjects();
}

Renderer::~Renderer() {
  delete this->deviceCollection;
}

void Renderer::render() {
  this->displayDevice->drawFrame();
}

void Renderer::updateUniformBuffers(Camera* camera) {
  this->displayDevice->updateUniformBuffer(0, camera->getUniformPointer(), camera->getUniformStructureSize());
}