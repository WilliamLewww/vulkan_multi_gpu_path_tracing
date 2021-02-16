#include "render_pass.h"

RenderPass::RenderPass(VkDevice logicalDevice, VkFormat swapchainImageFormat) {
  VkAttachmentDescription colorAttachment = {
    .flags = 0,
    .format = swapchainImageFormat,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
  };

  VkAttachmentDescription depthAttachment = {
    .flags = 0,
    .format = VK_FORMAT_D32_SFLOAT,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
  };

  VkAttachmentReference colorAttachmentRef = {
    .attachment = 0,
    .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
  };

  VkAttachmentReference depthAttachmentRef = {
    .attachment = 1,
    .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
  };

  VkSubpassDescription firstSubpass = {
    .flags = 0,
    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    .inputAttachmentCount = 0,
    .pInputAttachments = NULL,
    .colorAttachmentCount = 1,
    .pColorAttachments = &colorAttachmentRef,
    .pResolveAttachments = NULL,
    .pDepthStencilAttachment = &depthAttachmentRef,
    .preserveAttachmentCount = 0,
    .pPreserveAttachments = NULL
  };

  VkSubpassDescription secondSubpass = {
    .flags = 0,
    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    .inputAttachmentCount = 0,
    .pInputAttachments = NULL,
    .colorAttachmentCount = 1,
    .pColorAttachments = &colorAttachmentRef,
    .pResolveAttachments = NULL,
    .pDepthStencilAttachment = &depthAttachmentRef,
    .preserveAttachmentCount = 0,
    .pPreserveAttachments = NULL
  };

  VkSubpassDescription thirdSubpass = {
    .flags = 0,
    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    .inputAttachmentCount = 0,
    .pInputAttachments = NULL,
    .colorAttachmentCount = 1,
    .pColorAttachments = &colorAttachmentRef,
    .pResolveAttachments = NULL,
    .pDepthStencilAttachment = &depthAttachmentRef,
    .preserveAttachmentCount = 0,
    .pPreserveAttachments = NULL
  };

  VkSubpassDescription fourthSubpass = {
    .flags = 0,
    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    .inputAttachmentCount = 0,
    .pInputAttachments = NULL,
    .colorAttachmentCount = 1,
    .pColorAttachments = &colorAttachmentRef,
    .pResolveAttachments = NULL,
    .pDepthStencilAttachment = &depthAttachmentRef,
    .preserveAttachmentCount = 0,
    .pPreserveAttachments = NULL
  };

  std::vector<VkSubpassDescription> subpassList = {firstSubpass, secondSubpass, thirdSubpass, fourthSubpass};

  VkSubpassDependency firstDependency = {
    .srcSubpass = VK_SUBPASS_EXTERNAL,
    .dstSubpass = 0,
    .srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    .dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    .srcAccessMask = 0,
    .dstAccessMask = 0,
    .dependencyFlags = 0
  };

  VkSubpassDependency secondDependency = {
    .srcSubpass = 0,
    .dstSubpass = 1,
    .srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
    .dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
    .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
    .dependencyFlags = 0
  };

  VkSubpassDependency thirdDependency = {
    .srcSubpass = 1,
    .dstSubpass = 2,
    .srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    .dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    .srcAccessMask = 0,
    .dstAccessMask = 0,
    .dependencyFlags = 0
  };

  VkSubpassDependency fourthDependency = {
    .srcSubpass = 2,
    .dstSubpass = 3,
    .srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    .dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    .srcAccessMask = 0,
    .dstAccessMask = 0,
    .dependencyFlags = 0
  };

  std::vector<VkSubpassDependency> dependencyList = {firstDependency, secondDependency, thirdDependency, fourthDependency};

  VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment};

  VkRenderPassCreateInfo renderPassInfo = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .attachmentCount = 2,
    .pAttachments = attachments,
    .subpassCount = (uint32_t)subpassList.size(),
    .pSubpasses = subpassList.data(),
    .dependencyCount = (uint32_t)dependencyList.size(),
    .pDependencies = dependencyList.data()
  };

  if (vkCreateRenderPass(logicalDevice, &renderPassInfo, NULL, &this->renderPass) != VK_SUCCESS) {
    printf("failed to create render pass\n");
  }
}

RenderPass::~RenderPass() {

}

VkRenderPass RenderPass::getRenderPass() {
  return this->renderPass;
}