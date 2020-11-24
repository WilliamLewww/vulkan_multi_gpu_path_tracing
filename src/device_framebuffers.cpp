#include "device_framebuffers.h"

DeviceFramebuffers::DeviceFramebuffers(VkDevice logicalDevice, 
                                       uint32_t swapchainImageCount, 
                                       VkExtent2D swapchainExtent,
                                       std::vector<VkImageView> swapchainImageViewList,
                                       VkRenderPass renderPass,
                                       VkImageView depthImageView) {

  this->framebufferList.resize(swapchainImageCount);

  for (int x = 0; x < swapchainImageCount; x++) {
    VkImageView attachments[2] = {
      swapchainImageViewList[x],
      depthImageView
    };

    VkFramebufferCreateInfo framebufferCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .renderPass = renderPass,
      .attachmentCount = 2,
      .pAttachments = attachments,
      .width = swapchainExtent.width,
      .height = swapchainExtent.height,
      .layers = 1
    };

    if (vkCreateFramebuffer(logicalDevice, &framebufferCreateInfo, NULL, &this->framebufferList[x]) != VK_SUCCESS) {
      printf("failed to create swapchain framebuffer #%d\n", x);
    }
  }
}

DeviceFramebuffers::~DeviceFramebuffers() {

}

std::vector<VkFramebuffer> DeviceFramebuffers::getFramebufferList() {
  return this->framebufferList;
}