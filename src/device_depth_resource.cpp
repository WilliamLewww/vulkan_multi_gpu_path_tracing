#include "device_depth_resource.h"

DeviceDepthResource::DeviceDepthResource(VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, VkExtent2D swapchainExtent) {
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
}

DeviceDepthResource::~DeviceDepthResource() {

}

VkImageView DeviceDepthResource::getDepthImageView() {
  return this->depthImageView;
}