#include "render_command_buffers.h"

RenderCommandBuffers::RenderCommandBuffers(VkDevice logicalDevice, 
                                         uint32_t swapchainImageCount, 
                                         VkExtent2D swapchainExtent, 
                                         VkCommandPool commandPool, 
                                         VkRenderPass renderPass, 
                                         std::vector<VkFramebuffer> framebufferList,
                                         std::vector<VkImage> swapchainImageList,
                                         VkImage rayTraceImage,
                                         std::vector<VkPipeline> pipelineList,
                                         std::vector<VkPipelineLayout> pipelineLayoutList,
                                         std::vector<VkDescriptorSet> descriptorSetList,
                                         std::vector<ModelInstance*> modelInstanceList) {

  this->commandBufferList.resize(swapchainImageCount);

  VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandPool = commandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandBufferCount = swapchainImageCount;

  if (vkAllocateCommandBuffers(logicalDevice, &commandBufferAllocateInfo, this->commandBufferList.data()) != VK_SUCCESS) {
    printf("failed to allocate command buffers\n");
  }

  for (int x = 0; x < swapchainImageCount; x++) {
    VkCommandBufferBeginInfo commandBufferBeginCreateInfo = {};
    commandBufferBeginCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = framebufferList[x];
    VkOffset2D renderAreaOffset = {0, 0};
    renderPassBeginInfo.renderArea.offset = renderAreaOffset;
    renderPassBeginInfo.renderArea.extent = swapchainExtent;

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

    if (vkBeginCommandBuffer(this->commandBufferList[x], &commandBufferBeginCreateInfo) != VK_SUCCESS) {
      printf("failed to begin recording command buffer for image #%d\n", x);
    }

    vkCmdBeginRenderPass(this->commandBufferList[x], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    for (int z = 0; z < pipelineList.size(); z++) {
      vkCmdBindPipeline(this->commandBufferList[x], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineList[z]);
      vkCmdBindDescriptorSets(this->commandBufferList[x], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayoutList[z], 0, descriptorSetList.size(), descriptorSetList.data(), 0, 0);
      for (int y = 0; y < modelInstanceList.size(); y++) {
        VkDeviceSize offset = 0;
        std::vector<VkBuffer> vertexBufferList = {modelInstanceList[y]->getVertexBuffer()};
        vkCmdBindVertexBuffers(this->commandBufferList[x], 0, 1, vertexBufferList.data(), &offset);
        vkCmdBindIndexBuffer(this->commandBufferList[x], modelInstanceList[y]->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(this->commandBufferList[x], modelInstanceList[y]->getModel()->getPrimitiveCount() * 3, 1, 0, 0, y);
      }
      
      if (z < pipelineList.size() - 1) {
        vkCmdNextSubpass(this->commandBufferList[x], VK_SUBPASS_CONTENTS_INLINE);
      }
    }
    
    vkCmdEndRenderPass(this->commandBufferList[x]);

    { 
      VkImageMemoryBarrier imageMemoryBarrier = {};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.pNext = NULL;
      imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      imageMemoryBarrier.image = swapchainImageList[x];
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = 0;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

      vkCmdPipelineBarrier(this->commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    }

    { 
      VkImageMemoryBarrier imageMemoryBarrier = {};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.pNext = NULL;
      imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
      imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imageMemoryBarrier.image = rayTraceImage;
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = 0;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      vkCmdPipelineBarrier(this->commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
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
  
      vkCmdCopyImage(this->commandBufferList[x], swapchainImageList[x], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, rayTraceImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
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
      imageMemoryBarrier.image = swapchainImageList[x];
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      imageMemoryBarrier.dstAccessMask = 0;

      vkCmdPipelineBarrier(this->commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
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
      imageMemoryBarrier.image = rayTraceImage;
      imageMemoryBarrier.subresourceRange = subresourceRange;
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imageMemoryBarrier.dstAccessMask = 0;

      vkCmdPipelineBarrier(this->commandBufferList[x], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
    }

    if (vkEndCommandBuffer(this->commandBufferList[x]) != VK_SUCCESS) {
      printf("failed to end recording command buffer for image #%d\n", x);
    }
  }
}

RenderCommandBuffers::~RenderCommandBuffers() {

}

std::vector<VkCommandBuffer>& RenderCommandBuffers::getCommandBufferList() {
  return this->commandBufferList;
}