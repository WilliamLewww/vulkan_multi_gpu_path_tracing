#include "device_command_pool.h"

DeviceCommandPool::DeviceCommandPool(VkDevice logicalDevice, uint32_t queueIndex) {
  VkCommandPoolCreateInfo commandPoolCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .queueFamilyIndex = queueIndex
  };

  if (vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, NULL, &this->commandPool) != VK_SUCCESS) {
    printf("failed to create command pool\n");
  }
}

DeviceCommandPool::~DeviceCommandPool() {

}

VkCommandPool DeviceCommandPool::getCommandPool() {
  return this->commandPool;
}