#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdio.h>

class SynchronizationObjects {
private:
  std::vector<VkSemaphore> imageAvailableSemaphoreList;
  std::vector<VkSemaphore> renderFinishedSemaphoreList;
  std::vector<VkFence> inFlightFenceList;
  std::vector<VkFence> imageInFlightList;
public:
  SynchronizationObjects(VkDevice logicalDevice, uint32_t framesInFlight, uint32_t swapchainImageCount);
  ~SynchronizationObjects();
};