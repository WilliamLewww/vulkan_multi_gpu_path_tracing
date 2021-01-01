#include "synchronization_objects.h"

SynchronizationObjects::SynchronizationObjects(VkDevice logicalDevice, uint32_t framesInFlight, uint32_t swapchainImageCount) {
  this->imageAvailableSemaphoreList.resize(framesInFlight);
  this->renderFinishedSemaphoreList.resize(framesInFlight);
  this->inFlightFenceList.resize(framesInFlight);
  this->imageInFlightList.resize(swapchainImageCount);
  for (int x = 0; x < swapchainImageCount; x++) {
    this->imageInFlightList[x] = VK_NULL_HANDLE;
  }

  VkSemaphoreCreateInfo semaphoreCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
  };

  VkFenceCreateInfo fenceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = NULL,
    .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };

  for (int x = 0; x < framesInFlight; x++) {
    if (vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, NULL, &this->imageAvailableSemaphoreList[x]) != VK_SUCCESS ||
        vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, NULL, &this->renderFinishedSemaphoreList[x]) != VK_SUCCESS ||
        vkCreateFence(logicalDevice, &fenceCreateInfo, NULL, &this->inFlightFenceList[x]) != VK_SUCCESS) {
      printf("failed to create synchronization objects for frame #%d\n", x);
    }
  }
}

SynchronizationObjects::~SynchronizationObjects() {

}

VkSemaphore& SynchronizationObjects::getImageAvailableSemaphore(int index) {
  return this->imageAvailableSemaphoreList[index];
}

VkSemaphore& SynchronizationObjects::getRenderFinishedSemaphore(int index) {
  return this->renderFinishedSemaphoreList[index];
}

VkFence& SynchronizationObjects::getInFlightFence(int index) {
  return this->inFlightFenceList[index];
}

VkFence& SynchronizationObjects::getImageInFlight(int index) {
  return this->imageInFlightList[index];
}