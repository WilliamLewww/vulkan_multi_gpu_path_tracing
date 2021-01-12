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

  std::vector<std::map<Model*, std::vector<TRS>>> modelFrequencyMapList = {
    {
      {
        modelCollection->getModel(0), 
        {
          createTRS({0, 0, -3}, {1, 1, 1}),
        }
      },
    },
    {
      {
        modelCollection->getModel(2), 
        {
          createTRS({0, 0, -0.01}, {1, 1, 1}),
        }
      },
      {
        modelCollection->getModel(3), 
        {
          createTRS({0, 0, -0.5}, {1, 1, 1})
        }
      },
      {
        modelCollection->getModel(6), 
        {
          createTRS({0, 0, -0.62}, {1, 1, 1})
        }
      }
    },
  };
  this->displayDevice->createModelInstanceCollection(modelFrequencyMapList);
  this->displayDevice->createAccelerationStructureCollection();

  this->displayDevice->createStorageBuffers(this->displayDevice->getModelInstanceSet(1)->getModelInstanceIndex(modelCollection->getModel(3)), modelCollection->getModel(3)->getPrimitiveCount());

  std::map<void*, uint32_t> uniformBufferMap = {
    {camera->getUniformPointer(), camera->getUniformStructureSize()},
    {this->displayDevice->getModelInstanceSet(0)->getUniformBufferPointer(), this->displayDevice->getModelInstanceSet(0)->getUniformBufferSize()},
    {this->displayDevice->getModelInstanceSet(1)->getUniformBufferPointer(), this->displayDevice->getModelInstanceSet(1)->getUniformBufferSize()}
  };
  this->displayDevice->createUniformBufferCollection(uniformBufferMap);

  std::vector<std::vector<Descriptor*>> separatedDescriptorList = {
    {
      new Descriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getUniformBufferCollection()->getDescriptorBufferInfoPointer(0), NULL, NULL),
      new Descriptor(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getUniformBufferCollection()->getDescriptorBufferInfoPointer(1), NULL, NULL),
      new Descriptor(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getUniformBufferCollection()->getDescriptorBufferInfoPointer(2), NULL, NULL),
      new Descriptor(3, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, NULL, NULL, this->displayDevice->getAccelerationStructureSet(0)->getWriteDescriptorSetAccelerationStructurePointer()),
      new Descriptor(4, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, NULL, NULL, this->displayDevice->getAccelerationStructureSet(1)->getWriteDescriptorSetAccelerationStructurePointer()),
      new Descriptor(5, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), this->displayDevice->getTextures()->getDescriptorRayTraceImageInfoPointer(), NULL, NULL, NULL),
      new Descriptor(6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getStorageBuffers()->getDescriptorRayDirectionBufferInfoPointer(), NULL, NULL),
      new Descriptor(7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getStorageBuffers()->getDescriptorLensPropertiesBufferInfoPointer(), NULL, NULL),
    },
    {
      new Descriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(0)->getDescriptorTotalIndexBufferInfoPointer(), NULL, NULL),
      new Descriptor(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(0)->getDescriptorTotalVertexBufferInfoPointer(), NULL, NULL),
      new Descriptor(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(0)->getDescriptorTotalNormalIndexBufferInfoPointer(), NULL, NULL),
      new Descriptor(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(0)->getDescriptorTotalNormalBufferInfoPointer(), NULL, NULL),
      new Descriptor(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(0)->getDescriptorTotalMaterialIndexBufferInfoPointer(), NULL, NULL),
      new Descriptor(5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(0)->getDescriptorTotalMaterialBufferInfoPointer(), NULL, NULL),
      new Descriptor(6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(0)->getDescriptorTotalMaterialLightBufferInfoPointer(), NULL, NULL)
    },
    {
      new Descriptor(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(1)->getDescriptorTotalIndexBufferInfoPointer(), NULL, NULL),
      new Descriptor(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(1)->getDescriptorTotalVertexBufferInfoPointer(), NULL, NULL),
      new Descriptor(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(1)->getDescriptorTotalNormalIndexBufferInfoPointer(), NULL, NULL),
      new Descriptor(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(1)->getDescriptorTotalNormalBufferInfoPointer(), NULL, NULL),
      new Descriptor(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(1)->getDescriptorTotalMaterialIndexBufferInfoPointer(), NULL, NULL),
      new Descriptor(5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(1)->getDescriptorTotalMaterialBufferInfoPointer(), NULL, NULL),
      new Descriptor(6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (VkShaderStageFlagBits)(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT), NULL, this->displayDevice->getModelInstanceSet(1)->getDescriptorTotalMaterialLightBufferInfoPointer(), NULL, NULL)
    }
  };
  this->displayDevice->createDescriptorSetCollection(separatedDescriptorList);

  std::vector<std::vector<std::string>> shaderList = {
    {
      "bin/depth.vert.spv"
    },
    {
      "bin/lens.vert.spv",
      "bin/lens.frag.spv",
    },
    {
      "bin/basic.vert.spv",
      "bin/basic.frag.spv"
    },
    {
      "bin/minimap.vert.spv",
      "bin/minimap.frag.spv"
    }
  };

  std::vector<std::vector<float>> screenPropertiesList = {
    {
      0.0f,
      0.0f,
      800.0f,
      600.0f,
    },
    {
      0.0f,
      0.0f,
      800.0f,
      600.0f,
    },
    {
      0.0f,
      0.0f,
      800.0f,
      600.0f,
    },
    {
      600.0f,
      150.0f,
      200.0f,
      -150.0f,
    }
  };

  this->displayDevice->createGraphicsPipelineCollection(shaderList, screenPropertiesList);

  this->displayDevice->createRenderCommandBuffers();
  this->displayDevice->createSynchronizationObjects();
}

Renderer::~Renderer() {
  delete this->deviceCollection;
}

ModelInstanceSetCollection* Renderer::getModelInstanceSetCollection() {
  return this->displayDevice->getModelInstanceSetCollection();
}

ModelInstanceSet* Renderer::getModelInstanceSet(int index) {
  return this->displayDevice->getModelInstanceSet(index);
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

void Renderer::render(int index) {
  this->displayDevice->drawFrame(index);
}

void Renderer::updateCameraUniformBuffers(Camera* camera) {
  this->displayDevice->updateUniformBuffer(0, camera->getUniformPointer(), camera->getUniformStructureSize());
}

void Renderer::updateModelInstancesUniformBuffers(int binding, int index) {
  this->displayDevice->updateUniformBuffer(binding, this->displayDevice->getModelInstanceSet(index)->getUniformBufferPointer(), this->displayDevice->getModelInstanceSet(index)->getUniformBufferSize());
}

void Renderer::updateAccelerationStructure(int index) {
  this->displayDevice->updateAccelerationStructureSet(index);
}