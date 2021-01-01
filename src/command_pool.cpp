#include "command_pool.h"

CommandPool::CommandPool(VkDevice logicalDevice, uint32_t queueIndex) {
  VkCommandPoolCreateInfo commandPoolCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = NULL,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = queueIndex
  };

  if (vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, NULL, &this->commandPool) != VK_SUCCESS) {
    printf("failed to create command pool\n");
  }
}

CommandPool::~CommandPool() {

}

VkCommandPool CommandPool::getCommandPool() {
  return this->commandPool;
}