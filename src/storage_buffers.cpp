#include "storage_buffers.h"

double uniformRandom() { 
  return ((double)rand() / (double)RAND_MAX); 
}

StorageBuffers::StorageBuffers(uint32_t apertureInstanceIndex,
                               uint32_t aperturePrimitiveCount,
                               uint32_t aperturePrimitiveOffset,
                               uint32_t lastLensElementInstanceIndex,
                               uint32_t lastLensElementPrimitiveCount,
                               uint32_t filmInstanceIndex,
                               VkDevice logicalDevice, 
                               VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                               VkCommandPool commandPool,
                               VkQueue queue) {

  {
    srand(time(NULL));

    float randomData[800 * 600];
    for (int x = 0; x < 800 * 600; x++) {
      randomData[x] = uniformRandom();
    }

    VkDeviceSize bufferSize = sizeof(float) * 800 * 600;
  
    VkBuffer randomStagingBuffer;
    VkDeviceMemory randomStagingBufferMemory;
    BufferFactory::createBuffer(logicalDevice,
                                physicalDeviceMemoryProperties,
                                bufferSize, 
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                                &randomStagingBuffer, 
                                &randomStagingBufferMemory);

    void* randomDataDevice;
    vkMapMemory(logicalDevice, randomStagingBufferMemory, 0, bufferSize, 0, &randomDataDevice);
    memcpy(randomDataDevice, randomData, bufferSize);
    vkUnmapMemory(logicalDevice, randomStagingBufferMemory);

    BufferFactory::createBuffer(logicalDevice, 
                                physicalDeviceMemoryProperties,
                                bufferSize, 
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                                &this->randomBuffer, 
                                &this->randomBufferMemory);  

    BufferFactory::copyBuffer(logicalDevice, commandPool, queue, randomStagingBuffer, this->randomBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, randomStagingBuffer, NULL);
    vkFreeMemory(logicalDevice, randomStagingBufferMemory, NULL);
  }

  {
    VkDeviceSize bufferSize = sizeof(float) * 800 * 600 * 3;

    BufferFactory::createBuffer(logicalDevice, 
                                physicalDeviceMemoryProperties,
                                bufferSize, 
                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                                &this->rayDirectionBuffer, 
                                &this->rayDirectionBufferMemory);
  }

  {
    LensPropertiesUniform lensPropertiesUniform = {
      .apertureInstanceIndex = apertureInstanceIndex,
      .aperturePrimitiveCount = aperturePrimitiveCount,
      .aperturePrimitiveOffset = aperturePrimitiveOffset,
      .lastLensElementInstanceIndex = lastLensElementInstanceIndex,
      .lastLensElementPrimitiveCount = lastLensElementPrimitiveCount,
      .filmInstanceIndex = filmInstanceIndex,
    };

    VkDeviceSize bufferSize = sizeof(LensPropertiesUniform);
  
    VkBuffer lensPropertiesStagingBuffer;
    VkDeviceMemory lensPropertiesStagingBufferMemory;
    BufferFactory::createBuffer(logicalDevice,
                                physicalDeviceMemoryProperties,
                                bufferSize, 
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                                &lensPropertiesStagingBuffer, 
                                &lensPropertiesStagingBufferMemory);

    void* totalVertexData;
    vkMapMemory(logicalDevice, lensPropertiesStagingBufferMemory, 0, bufferSize, 0, &totalVertexData);
    memcpy(totalVertexData, &lensPropertiesUniform, bufferSize);
    vkUnmapMemory(logicalDevice, lensPropertiesStagingBufferMemory);

    BufferFactory::createBuffer(logicalDevice, 
                                physicalDeviceMemoryProperties,
                                bufferSize, 
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                                &this->lensPropertiesBuffer, 
                                &this->lensPropertiesBufferMemory);  

    BufferFactory::copyBuffer(logicalDevice, commandPool, queue, lensPropertiesStagingBuffer, this->lensPropertiesBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, lensPropertiesStagingBuffer, NULL);
    vkFreeMemory(logicalDevice, lensPropertiesStagingBufferMemory, NULL);
  }

  {
    VkDeviceSize bufferSize = sizeof(float) * 800 * 600;

    BufferFactory::createBuffer(logicalDevice, 
                                physicalDeviceMemoryProperties,
                                bufferSize, 
                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                                &this->flareBuffer, 
                                &this->flareBufferMemory);
  }

  this->descriptorRandomBufferInfo = {
    .buffer = this->randomBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->descriptorRayDirectionBufferInfo = {
    .buffer = this->rayDirectionBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->descriptorLensPropertiesBufferInfo = {
    .buffer = this->lensPropertiesBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };

  this->descriptorFlareBufferInfo = {
    .buffer = this->flareBuffer,
    .offset = 0,
    .range = VK_WHOLE_SIZE
  };
}

StorageBuffers::~StorageBuffers() {

}

VkDescriptorBufferInfo* StorageBuffers::getDescriptorRandomBufferInfoPointer() {
  return &this->descriptorRandomBufferInfo;
}

VkDescriptorBufferInfo* StorageBuffers::getDescriptorRayDirectionBufferInfoPointer() {
  return &this->descriptorRayDirectionBufferInfo;
}

VkDescriptorBufferInfo* StorageBuffers::getDescriptorLensPropertiesBufferInfoPointer() {
  return &this->descriptorLensPropertiesBufferInfo;
}

VkDescriptorBufferInfo* StorageBuffers::getDescriptorFlareBufferInfoPointer() {
  return &this->descriptorFlareBufferInfo;
}