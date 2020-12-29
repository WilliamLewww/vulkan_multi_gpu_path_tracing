#include "device_textures.h"

DeviceTextures::DeviceTextures(VkDevice logicalDevice, 
                               VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, 
                               VkFormat swapchainImageFormat,
                               VkExtent2D swapchainExtent, 
                               VkCommandPool commandPool, 
                               VkQueue queue) {

  VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

  ImageFactory::createImage(logicalDevice,
                            physicalDeviceMemoryProperties,
                            swapchainExtent.width, 
                            swapchainExtent.height, 
                            depthFormat, 
                            VK_IMAGE_TILING_OPTIMAL, 
                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                            &this->depthImage, 
                            &this->depthImageMemory);

  VkImageViewCreateInfo viewInfo = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .image = this->depthImage,
    .viewType = VK_IMAGE_VIEW_TYPE_2D,
    .format = depthFormat,
    .components = {},
    .subresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1
    },
  };

  if (vkCreateImageView(logicalDevice, &viewInfo, NULL, &this->depthImageView) != VK_SUCCESS) {
    printf("failed to create texture image view\n");
  }
  
  ImageFactory::createImage(logicalDevice,
                            physicalDeviceMemoryProperties,
                            800, 
                            600, 
                            swapchainImageFormat,
                            VK_IMAGE_TILING_OPTIMAL, 
                            VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, 
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                            &this->rayTraceImage, 
                            &this->rayTraceImageMemory);

  VkImageSubresourceRange subresourceRange = {
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .baseMipLevel = 0,
    .levelCount = 1,
    .baseArrayLayer = 0,
    .layerCount = 1,
  };

  VkImageViewCreateInfo imageViewCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .image = this->rayTraceImage,
    .viewType = VK_IMAGE_VIEW_TYPE_2D,
    .format = swapchainImageFormat,
    .components = {},
    .subresourceRange = subresourceRange,
  };

  if (vkCreateImageView(logicalDevice, &imageViewCreateInfo, NULL, &this->rayTraceImageView) != VK_SUCCESS) {
    printf("failed to create image view\n");
  }

  VkImageMemoryBarrier imageMemoryBarrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = NULL,
    .srcAccessMask = 0,
    .dstAccessMask = 0,
    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .newLayout = VK_IMAGE_LAYOUT_GENERAL,
    .srcQueueFamilyIndex = 0,
    .dstQueueFamilyIndex = 0,
    .image = this->rayTraceImage,
    .subresourceRange = subresourceRange,
  };

  VkCommandBufferAllocateInfo bufferAllocateInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = NULL,
    .commandPool = commandPool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(logicalDevice, &bufferAllocateInfo, &commandBuffer);
  
  VkCommandBufferBeginInfo commandBufferBeginInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = NULL,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    .pInheritanceInfo = NULL
  };
  
  vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
  vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = NULL,
    .waitSemaphoreCount = 0,
    .pWaitSemaphores = NULL,
    .pWaitDstStageMask = NULL,
    .commandBufferCount = 1,
    .pCommandBuffers = &commandBuffer,
    .signalSemaphoreCount = 0,
    .pSignalSemaphores = NULL,
  };

  vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);

  vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);

  this->descriptorRayTraceImageInfo = {
    .imageView = this->rayTraceImageView,
    .imageLayout = VK_IMAGE_LAYOUT_GENERAL
  };
}

DeviceTextures::~DeviceTextures() {

}

VkImageView DeviceTextures::getDepthImageView() {
  return this->depthImageView;
}

VkImage DeviceTextures::getRayTraceImage() {
  return this->rayTraceImage;
}

VkDescriptorImageInfo* DeviceTextures::getDescriptorRayTraceImageInfoPointer() {
  return &this->descriptorRayTraceImageInfo;
}