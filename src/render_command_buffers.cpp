#include "render_command_buffers.h"

RenderCommandBuffers::RenderCommandBuffers(VkDevice logicalDevice, 
                                           uint32_t swapchainImageCount, 
                                           VkCommandPool commandPool) {

  this->commandBufferList.resize(swapchainImageCount);

  VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandPool = commandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandBufferCount = swapchainImageCount;

  if (vkAllocateCommandBuffers(logicalDevice, &commandBufferAllocateInfo, this->commandBufferList.data()) != VK_SUCCESS) {
    printf("failed to allocate command buffers\n");
  }
}

RenderCommandBuffers::~RenderCommandBuffers() {

}

std::vector<VkCommandBuffer>& RenderCommandBuffers::getCommandBufferList() {
  return this->commandBufferList;
}

void RenderCommandBuffers::recreateCommandBuffer(uint32_t imageIndex,
                                                 VkExtent2D swapchainExtent, 
                                                 VkRenderPass renderPass, 
                                                 std::vector<VkFramebuffer> framebufferList,
                                                 std::vector<VkImage> swapchainImageList,
                                                 VkImage rayTraceImage,
                                                 std::vector<VkPipeline> pipelineList,
                                                 std::vector<VkPipelineLayout> pipelineLayoutList,
                                                 std::vector<VkDescriptorSet> descriptorSetList,
                                                 std::vector<ModelInstance*> modelInstanceList,
                                                 uint32_t modelInstanceOffset,
                                                 bool isActiveGUI) {

  VkCommandBufferBeginInfo commandBufferBeginCreateInfo = {};
  commandBufferBeginCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  VkRenderPassBeginInfo renderPassBeginInfo = {};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = renderPass;
  renderPassBeginInfo.framebuffer = framebufferList[imageIndex];
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

  if (vkBeginCommandBuffer(this->commandBufferList[imageIndex], &commandBufferBeginCreateInfo) != VK_SUCCESS) {
    printf("failed to begin recording command buffer for image #%d\n", imageIndex);
  }

  vkCmdBeginRenderPass(this->commandBufferList[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

  for (int z = 0; z < pipelineList.size(); z++) {
    vkCmdBindPipeline(this->commandBufferList[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineList[z]);
    vkCmdBindDescriptorSets(this->commandBufferList[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayoutList[z], 0, descriptorSetList.size(), descriptorSetList.data(), 0, 0);
    for (int y = 0; y < modelInstanceList.size(); y++) {
      VkDeviceSize offset = 0;
      std::vector<VkBuffer> vertexBufferList = {modelInstanceList[y]->getVertexBuffer()};
      vkCmdBindVertexBuffers(this->commandBufferList[imageIndex], 0, 1, vertexBufferList.data(), &offset);
      vkCmdBindIndexBuffer(this->commandBufferList[imageIndex], modelInstanceList[y]->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
      vkCmdDrawIndexed(this->commandBufferList[imageIndex], modelInstanceList[y]->getModel()->getPrimitiveCount() * 3, 1, 0, 0, modelInstanceOffset + y);
    }
    
    if (z < pipelineList.size() - 1) {
      vkCmdNextSubpass(this->commandBufferList[imageIndex], VK_SUBPASS_CONTENTS_INLINE);
    }
  }

  if (isActiveGUI) {
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, this->commandBufferList[imageIndex]);
  }
  
  vkCmdEndRenderPass(this->commandBufferList[imageIndex]);

  if (vkEndCommandBuffer(this->commandBufferList[imageIndex]) != VK_SUCCESS) {
    printf("failed to end recording command buffer for image #%d\n", imageIndex);
  }
}