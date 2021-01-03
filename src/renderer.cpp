#include "renderer.h"

Renderer::Renderer(VkInstance vulkanInstance, VkSurfaceKHR surface, ModelCollection* modelCollection, Camera* camera) {
  this->deviceCollection = new DeviceCollection(vulkanInstance);

  this->displayDevice = this->deviceCollection->getDevice(0);
  this->displayDevice->initializeDeviceQueue(surface);

  std::vector<const char*> deviceExtensionList {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME, 
    "VK_KHR_ray_query",
    "VK_KHR_acceleration_structure",
    "VK_KHR_spirv_1_4",
    "VK_KHR_shader_float_controls",
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
  this->displayDevice->createTextures();
  this->displayDevice->createFramebuffers();

  std::map<Model*, std::vector<TRS>> modelFrequencyMap = {
    {
      modelCollection->getModel(0), 
      {
        createTRS({0, 0, 0}, {1, 1, 1})
      }
    },
    {
      modelCollection->getModel(2), 
      {
        createTRS({0, 0, 0}, {1, 1, 1})
      }
    },
    {
      modelCollection->getModel(1), 
      {
        createTRS({0, 0, 5}, {1, 1, 1})
      }
    }
  };
  this->displayDevice->createModelInstances(modelFrequencyMap);

  std::map<void*, uint32_t> uniformBufferMap = {
    {camera->getUniformPointer(), camera->getUniformStructureSize()},
    {this->displayDevice->getModelInstanceCollectionPointer()->getUniformBufferPointer(), this->displayDevice->getModelInstanceCollectionPointer()->getUniformBufferSize()}
  };
  this->displayDevice->createUniformBufferCollection(uniformBufferMap);

  this->displayDevice->createAccelerationStructureCollection(this->displayDevice->getModelInstanceCollectionPointer()->getModelInstanceMap());

  std::vector<std::vector<Descriptor*>> separatedDescriptorList = {
    {
      new Descriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getUniformBufferCollection()->getDescriptorBufferInfoPointer(0), NULL, NULL),
      new Descriptor(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getUniformBufferCollection()->getDescriptorBufferInfoPointer(1), NULL, NULL),
      new Descriptor(2, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, NULL, NULL, this->displayDevice->getAccelerationStructureCollection()->getWriteDescriptorSetAccelerationStructurePointer()),
      new Descriptor(3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), this->displayDevice->getTextures()->getDescriptorRayTraceImageInfoPointer(), NULL, NULL, NULL),
    },
    {
      new Descriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalIndexBufferInfoPointer(), NULL, NULL),
      new Descriptor(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalVertexBufferInfoPointer(), NULL, NULL),
      new Descriptor(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalNormalIndexBufferInfoPointer(), NULL, NULL),
      new Descriptor(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalNormalBufferInfoPointer(), NULL, NULL),
      new Descriptor(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalMaterialIndexBufferInfoPointer(), NULL, NULL),
      new Descriptor(5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalMaterialBufferInfoPointer(), NULL, NULL),
      new Descriptor(6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceCollection()->getDescriptorTotalMaterialLightBufferInfoPointer(), NULL, NULL)
    }
  };
  this->displayDevice->createDescriptorSetCollection(separatedDescriptorList);

  std::vector<std::vector<std::string>> shaderList = {
    {
      "bin/depth.vert.spv"
    },
    {
      "bin/basic.vert.spv",
      "bin/basic.frag.spv"
    }
  };

  this->displayDevice->createGraphicsPipelineCollection(shaderList);

  this->displayDevice->createRenderCommandBuffers();
  this->displayDevice->createSynchronizationObjects();
}

Renderer::~Renderer() {
  delete this->deviceCollection;
}

ModelInstanceCollection* Renderer::getModelInstanceCollection() {
  return this->displayDevice->getModelInstanceCollectionPointer();
}

VkPhysicalDevice Renderer::getPhysicalDevice() {
  return this->displayDevice->getPhysicalDevice();
}

VkDevice Renderer::getLogicalDevice() {
  return this->displayDevice->getLogicalDevice();
}

uint32_t Renderer::getGraphicsQueueIndex() {
  return this->displayDevice->getGraphicsQueueIndex();
}

VkQueue Renderer::getGraphicsQueue() {
  return this->displayDevice->getGraphicsQueue();
}

uint32_t Renderer::getMinImageCount() {
  return this->displayDevice->getMinImageCount();
}

uint32_t Renderer::getImageCount() {
  return this->displayDevice->getImageCount();
}

VkRenderPass Renderer::getRenderPass() {
  return this->displayDevice->getRenderPass();
}

VkCommandPool Renderer::getCommandPool() {
  return this->displayDevice->getCommandPool();
}

void Renderer::render() {
  this->displayDevice->drawFrame();
}

void Renderer::updateUniformBuffers(Camera* camera) {
  this->displayDevice->updateUniformBuffer(0, camera->getUniformPointer(), camera->getUniformStructureSize());
}