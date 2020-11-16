#include "command_buffer_manager.h"

CommandBufferManager::CommandBufferManager() {

}

CommandBufferManager::~CommandBufferManager() {

}

std::vector<VkCommandBuffer> CommandBufferManager::getCommandBufferList(Device* device) {
  return this->deviceMap[device].commandBufferList;
}

void CommandBufferManager::initializeContainerOnDevice(Device* device) {
  this->deviceMap.insert(std::pair<Device*, DeviceContainer>(device, DeviceContainer()));
}

void CommandBufferManager::createCommandBuffers(Device* device,
                                                std::vector<ModelInstance> modelInstanceList,
                                                VkPipeline pipeline, 
                                                VkPipelineLayout pipelineLayout, 
                                                std::vector<VkDescriptorSet>& descriptorSetList) {

  this->deviceMap[device].commandBufferList.resize(device->getSwapchainImageCount());
  
  VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandPool = device->getCommandPool();
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandBufferCount = device->getSwapchainImageCount();

  if (vkAllocateCommandBuffers(device->getLogicalDevice(), &commandBufferAllocateInfo, this->deviceMap[device].commandBufferList.data()) != VK_SUCCESS) {
    printf("failed to allocate command buffers\n");
  }

  for (int x = 0; x < device->getSwapchainImageCount(); x++) {
    VkCommandBufferBeginInfo commandBufferBeginCreateInfo = {};
    commandBufferBeginCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = device->getRenderPass();
    renderPassBeginInfo.framebuffer = device->getFramebufferList()[x];
    VkOffset2D renderAreaOffset = {0, 0};
    renderPassBeginInfo.renderArea.offset = renderAreaOffset;
    renderPassBeginInfo.renderArea.extent = device->getSwapchainExtent();

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

    if (vkBeginCommandBuffer(this->deviceMap[device].commandBufferList[x], &commandBufferBeginCreateInfo) != VK_SUCCESS) {
      printf("failed to begin recording command buffer for image #%d\n", x);
    }

    vkCmdBeginRenderPass(this->deviceMap[device].commandBufferList[x], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(this->deviceMap[device].commandBufferList[x], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    vkCmdBindDescriptorSets(this->deviceMap[device].commandBufferList[x], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 2, descriptorSetList.data(), 0, 0);

    for (int y = 0; y < modelInstanceList.size(); y++) {
      VkDeviceSize offset = 0;
      vkCmdBindVertexBuffers(this->deviceMap[device].commandBufferList[x], 0, 1, &modelInstanceList[y].vertexBuffer, &offset);
      vkCmdBindIndexBuffer(this->deviceMap[device].commandBufferList[x], modelInstanceList[y].indexBuffer, 0, VK_INDEX_TYPE_UINT32);
      vkCmdDrawIndexed(this->deviceMap[device].commandBufferList[x], modelInstanceList[y].primitiveCount * 3, 1, 0, 0, y);
    }
    
    vkCmdEndRenderPass(this->deviceMap[device].commandBufferList[x]);

    { 
      VkImageMemoryBarrier imageMemoryBarrier = {};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.pNext = NULL;
      imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      imageMemoryBarrier.image = device->getSwapchainImageList()[x];
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = 0;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

      vkCmdPipelineBarrier(this->deviceMap[device].commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    }

    { 
      VkImageMemoryBarrier imageMemoryBarrier = {};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.pNext = NULL;
      imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
      imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imageMemoryBarrier.image = device->getRayTraceImage();
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = 0;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      vkCmdPipelineBarrier(this->deviceMap[device].commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    }

    {
      VkImageSubresourceLayers subresourceLayers = {};
      subresourceLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      subresourceLayers.mipLevel = 0;
      subresourceLayers.baseArrayLayer = 0;
      subresourceLayers.layerCount = 1;

      VkOffset3D offset = {};
      offset.x = 0;
      offset.y = 0;
      offset.z = 0;

      VkExtent3D extent = {};
      extent.width = 800;
      extent.height = 600;
      extent.depth = 1;

      VkImageCopy imageCopy = {};
      imageCopy.srcSubresource = subresourceLayers;
      imageCopy.srcOffset = offset;
      imageCopy.dstSubresource = subresourceLayers;
      imageCopy.dstOffset = offset;
      imageCopy.extent = extent;
  
      vkCmdCopyImage(this->deviceMap[device].commandBufferList[x], device->getSwapchainImageList()[x], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, device->getRayTraceImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
    }

    { 
      VkImageSubresourceRange subresourceRange = {};
      subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      subresourceRange.baseMipLevel = 0;
      subresourceRange.levelCount = 1;
      subresourceRange.baseArrayLayer = 0;
      subresourceRange.layerCount = 1;

      VkImageMemoryBarrier imageMemoryBarrier = {};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.pNext = NULL;
      imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      imageMemoryBarrier.image = device->getSwapchainImageList()[x];
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      imageMemoryBarrier.dstAccessMask = 0;

      vkCmdPipelineBarrier(this->deviceMap[device].commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    }

    { 
      VkImageSubresourceRange subresourceRange = {};
      subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      subresourceRange.baseMipLevel = 0;
      subresourceRange.levelCount = 1;
      subresourceRange.baseArrayLayer = 0;
      subresourceRange.layerCount = 1;

      VkImageMemoryBarrier imageMemoryBarrier = {};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.pNext = NULL;
      imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
      imageMemoryBarrier.image = device->getRayTraceImage();
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imageMemoryBarrier.dstAccessMask = 0;

      vkCmdPipelineBarrier(this->deviceMap[device].commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    }

    if (vkEndCommandBuffer(this->deviceMap[device].commandBufferList[x]) != VK_SUCCESS) {
      printf("failed to end recording command buffer for image #%d\n", x);
    }
  }
}