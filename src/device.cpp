#include "device.h"

Device::Device(VkPhysicalDevice physicalDevice) {
  this->currentFrame = 0;
  this->framesInFlight = 2;

  this->physicalDevice = physicalDevice;

  vkGetPhysicalDeviceProperties(physicalDevice, &this->physicalDeviceProperties);
  vkGetPhysicalDeviceFeatures(physicalDevice, &this->physicalDeviceFeatures);
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &this->physicalDeviceMemoryProperties);
}

Device::~Device() {

}

VkPhysicalDevice Device::getPhysicalDevice() {
  return this->physicalDevice;
}

VkDevice Device::getLogicalDevice() {
  return this->logicalDevice;
}

uint32_t Device::getGraphicsQueueIndex() {
  return this->deviceQueue->getGraphicsQueueIndex();
}

VkQueue Device::getGraphicsQueue() {
  return this->deviceQueue->getGraphicsQueue();
}

uint32_t Device::getMinImageCount() {
  return 2;
}

uint32_t Device::getImageCount() {
  return this->swapchain->getSwapchainImageCount();
}

VkRenderPass Device::getRenderPass() {
  return this->renderPass->getRenderPass();
}

VkCommandPool Device::getCommandPool() {
  return this->commandPool->getCommandPool();
}

ModelInstanceCollection* Device::getModelInstanceCollectionPointer() {
  return this->modelInstanceCollection;
}

UniformBufferCollection* Device::getUniformBufferCollection() {
  return this->uniformBufferCollection;
}

AccelerationStructureCollection* Device::getAccelerationStructureCollection() {
  return this->accelerationStructureCollection;
}

Textures* Device::getTextures() {
  return this->textures;
}

ModelInstanceCollection* Device::getModelInstanceCollection() {
  return this->modelInstanceCollection;
}

void Device::initializeDeviceQueue(VkSurfaceKHR surface) {
  this->deviceQueue = new DeviceQueue(this->physicalDevice, surface);
}

void Device::createLogicalDevice(std::vector<const char*> extensions) {
  std::map<uint32_t, uint32_t> queueFrequencyMap = this->deviceQueue->getQueueFrequencyMap();

  std::vector<float> queuePriorityList = {1.0f, 1.0f};
  uint32_t deviceQueueCreateInfoCount = queueFrequencyMap.size();

  std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoList;

  for (std::pair<uint32_t, uint32_t> pair : queueFrequencyMap) {
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .queueFamilyIndex = pair.first,
      .queueCount = pair.second,
      .pQueuePriorities = queuePriorityList.data()
    };

    deviceQueueCreateInfoList.push_back(deviceQueueCreateInfo);
  }

  VkPhysicalDeviceBufferDeviceAddressFeaturesEXT bufferDeviceAddressFeatures = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT,
    .pNext = NULL,
    .bufferDeviceAddress = VK_TRUE,
    .bufferDeviceAddressCaptureReplay = VK_FALSE,
    .bufferDeviceAddressMultiDevice = VK_FALSE
  };

  VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
    .pNext = &bufferDeviceAddressFeatures,
    .rayQuery = VK_TRUE
  };

  VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
    .pNext = &rayQueryFeatures,
    .accelerationStructure = VK_TRUE,
    .accelerationStructureCaptureReplay = VK_TRUE,
    .accelerationStructureIndirectBuild = VK_FALSE,
    .accelerationStructureHostCommands = VK_FALSE,
    .descriptorBindingAccelerationStructureUpdateAfterBind = VK_FALSE
  };

  // incomplete listing due to large amount of data members
  VkPhysicalDeviceFeatures deviceFeatures = {
    .geometryShader = VK_TRUE,
    .fragmentStoresAndAtomics = VK_TRUE
  };

  VkDeviceCreateInfo deviceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &accelerationStructureFeatures,
    .flags = 0,
    .queueCreateInfoCount = deviceQueueCreateInfoCount,
    .pQueueCreateInfos = deviceQueueCreateInfoList.data(),
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = NULL,
    .enabledExtensionCount = (uint32_t)extensions.size(),
    .ppEnabledExtensionNames = extensions.data(),
    .pEnabledFeatures = &deviceFeatures
  };

  if (vkCreateDevice(this->physicalDevice, &deviceCreateInfo, NULL, &this->logicalDevice) != VK_SUCCESS) {
    printf("failed to created logical connection to device\n");
  }

  this->deviceQueue->setQueueHandles(this->logicalDevice);
}

void Device::createCommandPool() {
  this->commandPool = new CommandPool(this->logicalDevice, this->deviceQueue->getGraphicsQueueIndex());
}

void Device::createSwapchain(VkSurfaceKHR surface) {
  this->swapchain = new Swapchain(this->logicalDevice, this->physicalDevice, surface, this->deviceQueue->getGraphicsQueueIndex(), this->deviceQueue->getPresentQueueIndex());
}

void Device::createRenderPass() {
  this->renderPass = new RenderPass(this->logicalDevice, this->swapchain->getSwapchainImageFormat());
}

void Device::createTextures() {
  this->textures = new Textures(this->logicalDevice, 
                                            this->physicalDeviceMemoryProperties, 
                                            this->swapchain->getSwapchainImageFormat(),
                                            this->swapchain->getSwapchainExtent(),
                                            this->commandPool->getCommandPool(), 
                                            this->deviceQueue->getGraphicsQueue());
}

void Device::createFramebuffers() {
  this->framebuffers = new Framebuffers(this->logicalDevice, 
                                                    this->swapchain->getSwapchainImageCount(), 
                                                    this->swapchain->getSwapchainExtent(),
                                                    this->swapchain->getSwapchainImageViewList(),
                                                    this->renderPass->getRenderPass(),
                                                    this->textures->getDepthImageView());
}

void Device::createModelInstances(std::map<Model*, std::vector<Matrix4x4>> modelFrequencyMap) {
  this->modelInstanceCollection = new ModelInstanceCollection(modelFrequencyMap,
                                                              this->logicalDevice, 
                                                              this->physicalDeviceMemoryProperties, 
                                                              this->commandPool->getCommandPool(),
                                                              this->deviceQueue->getGraphicsQueue());
}

void Device::createUniformBufferCollection(std::map<void*, uint32_t> bufferMap) {
  this->uniformBufferCollection = new UniformBufferCollection(bufferMap, this->logicalDevice, this->physicalDeviceMemoryProperties);
}

void Device::createAccelerationStructureCollection(std::map<Model*, std::vector<ModelInstance*>> modelInstanceMap) {
  this->accelerationStructureCollection = new AccelerationStructureCollection(modelInstanceMap, this->logicalDevice, this->physicalDeviceMemoryProperties, this->commandPool->getCommandPool(), this->deviceQueue->getComputeQueue());
}

void Device::createDescriptorSetCollection(std::vector<std::vector<Descriptor*>> separatedDescriptorList) {
  this->descriptorSetCollection = new DescriptorSetCollection(separatedDescriptorList, this->logicalDevice);
}

void Device::createGraphicsPipelineCollection(std::vector<std::vector<std::string>> shaderList) {
  this->graphicsPipelineCollection = new GraphicsPipelineCollection(shaderList, this->descriptorSetCollection->getDescriptorSetLayoutList(), this->logicalDevice, this->swapchain->getSwapchainExtent(), this->renderPass->getRenderPass());
}

void Device::createRenderCommandBuffers() {
  this->renderCommandBuffers = new RenderCommandBuffers(this->logicalDevice, 
                                                        this->swapchain->getSwapchainImageCount(), 
                                                        this->swapchain->getSwapchainExtent(), 
                                                        this->commandPool->getCommandPool(), 
                                                        this->renderPass->getRenderPass(), 
                                                        this->framebuffers->getFramebufferList(),
                                                        this->swapchain->getSwapchainImageList(),
                                                        this->textures->getRayTraceImage(),
                                                        this->graphicsPipelineCollection->getGraphicsPipelineList(),
                                                        this->graphicsPipelineCollection->getPipelineLayoutList(),
                                                        this->descriptorSetCollection->getDescriptorSetList(),
                                                        this->modelInstanceCollection->getModelInstanceList());
}

void Device::createSynchronizationObjects() {
  this->synchronizationObjects = new SynchronizationObjects(this->logicalDevice, this->framesInFlight, this->swapchain->getSwapchainImageCount());
}

void Device::drawFrame() {
  vkWaitForFences(this->logicalDevice, 1, &this->synchronizationObjects->getInFlightFence(this->currentFrame), VK_TRUE, UINT64_MAX);
    
  uint32_t imageIndex;
  vkAcquireNextImageKHR(this->logicalDevice, this->swapchain->getSwapchain(), UINT64_MAX, this->synchronizationObjects->getImageAvailableSemaphore(this->currentFrame), VK_NULL_HANDLE, &imageIndex);
    
  if (this->synchronizationObjects->getImageInFlight(imageIndex) != VK_NULL_HANDLE) {
    vkWaitForFences(this->logicalDevice, 1, &this->synchronizationObjects->getImageInFlight(imageIndex), VK_TRUE, UINT64_MAX);
  }
  this->synchronizationObjects->getImageInFlight(imageIndex) = this->synchronizationObjects->getInFlightFence(this->currentFrame);

  VkSemaphore waitSemaphores[1] = {this->synchronizationObjects->getImageAvailableSemaphore(this->currentFrame)};
  VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signalSemaphores[1] = {this->synchronizationObjects->getRenderFinishedSemaphore(this->currentFrame)};
   
  VkSubmitInfo submitInfo = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = NULL,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = waitSemaphores,
    .pWaitDstStageMask = waitStages,
    .commandBufferCount = 1,
    .pCommandBuffers = &this->renderCommandBuffers->getCommandBufferList()[imageIndex],
    .signalSemaphoreCount = 1,
    .pSignalSemaphores = signalSemaphores,
  };

  vkResetFences(this->logicalDevice, 1, &this->synchronizationObjects->getInFlightFence(this->currentFrame));

  {
    VkCommandBufferBeginInfo commandBufferBeginCreateInfo = {};
    commandBufferBeginCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = this->renderPass->getRenderPass();
    renderPassBeginInfo.framebuffer = this->framebuffers->getFramebufferList()[imageIndex];
    VkOffset2D renderAreaOffset = {0, 0};
    renderPassBeginInfo.renderArea.offset = renderAreaOffset;
    renderPassBeginInfo.renderArea.extent = this->swapchain->getSwapchainExtent();

    VkClearValue clearValues[2] = {
      {.color = {0.0f, 0.0f, 0.0f, 1.0f}},
      {.depthStencil = {1.0f, 0}}
    };

    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    if (vkBeginCommandBuffer(this->renderCommandBuffers->getCommandBufferList()[imageIndex], &commandBufferBeginCreateInfo) != VK_SUCCESS) {
      printf("failed to begin recording command buffer for image #%d\n", imageIndex);
    }

    vkCmdBeginRenderPass(this->renderCommandBuffers->getCommandBufferList()[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    for (int z = 0; z < this->graphicsPipelineCollection->getGraphicsPipelineList().size(); z++) {
      vkCmdBindPipeline(this->renderCommandBuffers->getCommandBufferList()[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipelineCollection->getGraphicsPipelineList()[z]);
      vkCmdBindDescriptorSets(this->renderCommandBuffers->getCommandBufferList()[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipelineCollection->getPipelineLayoutList()[z], 0, this->descriptorSetCollection->getDescriptorSetList().size(), this->descriptorSetCollection->getDescriptorSetList().data(), 0, 0);
      for (int y = 0; y < this->modelInstanceCollection->getModelInstanceList().size(); y++) {
        VkDeviceSize offset = 0;
        std::vector<VkBuffer> vertexBufferList = {this->modelInstanceCollection->getModelInstanceList()[y]->getVertexBuffer()};
        vkCmdBindVertexBuffers(this->renderCommandBuffers->getCommandBufferList()[imageIndex], 0, 1, vertexBufferList.data(), &offset);
        vkCmdBindIndexBuffer(this->renderCommandBuffers->getCommandBufferList()[imageIndex], this->modelInstanceCollection->getModelInstanceList()[y]->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(this->renderCommandBuffers->getCommandBufferList()[imageIndex], this->modelInstanceCollection->getModelInstanceList()[y]->getModel()->getPrimitiveCount() * 3, 1, 0, 0, y);
      }
      
      if (z < this->graphicsPipelineCollection->getGraphicsPipelineList().size() - 1) {
        vkCmdNextSubpass(this->renderCommandBuffers->getCommandBufferList()[imageIndex], VK_SUBPASS_CONTENTS_INLINE);
      }
    }

    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, this->renderCommandBuffers->getCommandBufferList()[imageIndex]);
    
    vkCmdEndRenderPass(this->renderCommandBuffers->getCommandBufferList()[imageIndex]);

    // { 
    //   VkImageMemoryBarrier imageMemoryBarrier = {};
    //   imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //   imageMemoryBarrier.pNext = NULL;
    //   imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //   imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    //   imageMemoryBarrier.image = swapchainImageList[imageIndex];
    //   imageMemoryBarrier.subresourceRange = subresourceRange;
    //   imageMemoryBarrier.srcAccessMask = 0;
    //   imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    //   vkCmdPipelineBarrier(this->renderCommandBuffers->getCommandBufferList()[imageIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    // }

    // { 
    //   VkImageMemoryBarrier imageMemoryBarrier = {};
    //   imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //   imageMemoryBarrier.pNext = NULL;
    //   imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    //   imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //   imageMemoryBarrier.image = rayTraceImage;
    //   imageMemoryBarrier.subresourceRange = subresourceRange;
    //   imageMemoryBarrier.srcAccessMask = 0;
    //   imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    //   vkCmdPipelineBarrier(this->renderCommandBuffers->getCommandBufferList()[imageIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    // }

    // {
    //   VkImageSubresourceLayers subresourceLayers = {};
    //   subresourceLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //   subresourceLayers.mipLevel = 0;
    //   subresourceLayers.baseArrayLayer = 0;
    //   subresourceLayers.layerCount = 1;

    //   VkOffset3D offset = {};
    //   offset.x = 0;
    //   offset.y = 0;
    //   offset.z = 0;

    //   VkExtent3D extent = {};
    //   extent.width = 800;
    //   extent.height = 600;
    //   extent.depth = 1;

    //   VkImageCopy imageCopy = {};
    //   imageCopy.srcSubresource = subresourceLayers;
    //   imageCopy.srcOffset = offset;
    //   imageCopy.dstSubresource = subresourceLayers;
    //   imageCopy.dstOffset = offset;
    //   imageCopy.extent = extent;
  
    //   vkCmdCopyImage(this->renderCommandBuffers->getCommandBufferList()[imageIndex], swapchainImageList[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, rayTraceImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
    // }

    // { 
    //   VkImageSubresourceRange subresourceRange = {};
    //   subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //   subresourceRange.baseMipLevel = 0;
    //   subresourceRange.levelCount = 1;
    //   subresourceRange.baseArrayLayer = 0;
    //   subresourceRange.layerCount = 1;

    //   VkImageMemoryBarrier imageMemoryBarrier = {};
    //   imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //   imageMemoryBarrier.pNext = NULL;
    //   imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    //   imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    //   imageMemoryBarrier.image = swapchainImageList[imageIndex];
    //   imageMemoryBarrier.subresourceRange = subresourceRange;
    //   imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    //   imageMemoryBarrier.dstAccessMask = 0;

    //   vkCmdPipelineBarrier(this->renderCommandBuffers->getCommandBufferList()[imageIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    // }

    // { 
    //   VkImageSubresourceRange subresourceRange = {};
    //   subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //   subresourceRange.baseMipLevel = 0;
    //   subresourceRange.levelCount = 1;
    //   subresourceRange.baseArrayLayer = 0;
    //   subresourceRange.layerCount = 1;

    //   VkImageMemoryBarrier imageMemoryBarrier = {};
    //   imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //   imageMemoryBarrier.pNext = NULL;
    //   imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //   imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    //   imageMemoryBarrier.image = rayTraceImage;
    //   imageMemoryBarrier.subresourceRange = subresourceRange;
    //   imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    //   imageMemoryBarrier.dstAccessMask = 0;

    //   vkCmdPipelineBarrier(this->renderCommandBuffers->getCommandBufferList()[imageIndex], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    // }

    if (vkEndCommandBuffer(this->renderCommandBuffers->getCommandBufferList()[imageIndex]) != VK_SUCCESS) {
      printf("failed to end recording command buffer for image #%d\n", imageIndex);
    }
  }

  if (vkQueueSubmit(this->deviceQueue->getGraphicsQueue(), 1, &submitInfo, this->synchronizationObjects->getInFlightFence(this->currentFrame)) != VK_SUCCESS) {
    printf("failed to submit draw command buffer\n");
  }

  VkSwapchainKHR swapchains[1] = {this->swapchain->getSwapchain()};

  VkPresentInfoKHR presentInfo = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pNext = NULL,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = signalSemaphores,
    .swapchainCount = 1,
    .pSwapchains = swapchains,
    .pImageIndices = &imageIndex,
    .pResults = NULL,
  };

  vkQueuePresentKHR(this->deviceQueue->getPresentQueue(), &presentInfo);

  this->currentFrame = (this->currentFrame + 1) % this->framesInFlight;
}

void Device::updateUniformBuffer(int index, void* buffer, uint32_t bufferSize) {
  this->uniformBufferCollection->updateUniformBuffer(this->logicalDevice, index, buffer, bufferSize);
}