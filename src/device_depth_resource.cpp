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

  VkImageViewCreateInfo viewInfo = {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = this->depthImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = depthFormat;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(logicalDevice, &viewInfo, NULL, &this->depthImageView) != VK_SUCCESS) {
    printf("failed to create texture image view\n");
  }
}

DeviceDepthResource::~DeviceDepthResource() {

}