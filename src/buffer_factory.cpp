#include "buffer_factory.h"

void BufferFactory::createBuffer(VkDevice logicalDevice,
                                 VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties,
                                 VkDeviceSize size, 
                                 VkBufferUsageFlags usageFlags, 
                                 VkMemoryPropertyFlags propertyFlags, 
                                 VkBuffer* buffer, 
                                 VkDeviceMemory* bufferMemory) {

  VkBufferCreateInfo bufferCreateInfo = {};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = size;
  bufferCreateInfo.usage = usageFlags;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(logicalDevice, &bufferCreateInfo, NULL, buffer) != VK_SUCCESS) {
    printf("failed to create buffer\n");
  }

  VkMemoryRequirements memoryRequirements;
  vkGetBufferMemoryRequirements(logicalDevice, *buffer, &memoryRequirements);

  VkMemoryAllocateInfo memoryAllocateInfo = {};
  memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memoryAllocateInfo.allocationSize = memoryRequirements.size;

  uint32_t memoryTypeIndex = -1;
  for (int x = 0; x < physicalDeviceMemoryProperties.memoryTypeCount; x++) {
    if ((memoryRequirements.memoryTypeBits & (1 << x)) && (physicalDeviceMemoryProperties.memoryTypes[x].propertyFlags & propertyFlags) == propertyFlags) {
      memoryTypeIndex = x;
      break;
    }
  }
  memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

  if (vkAllocateMemory(logicalDevice, &memoryAllocateInfo, NULL, bufferMemory) != VK_SUCCESS) {
    printf("failed to allocate buffer memory\n");
  }

  vkBindBufferMemory(logicalDevice, *buffer, *bufferMemory, 0);
}

void BufferFactory::copyBuffer(VkDevice logicalDevice,
                               VkCommandPool commandPool,
                               VkQueue queue,
                               VkBuffer srcBuffer,
                               VkBuffer dstBuffer, 
                               VkDeviceSize size) {

  VkCommandBufferAllocateInfo bufferAllocateInfo = {};
  bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  bufferAllocateInfo.commandPool = commandPool;
  bufferAllocateInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(logicalDevice, &bufferAllocateInfo, &commandBuffer);
  
  VkCommandBufferBeginInfo commandBufferBeginInfo = {};
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  
  vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
  VkBufferCopy bufferCopy = {};
  bufferCopy.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);

  vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}