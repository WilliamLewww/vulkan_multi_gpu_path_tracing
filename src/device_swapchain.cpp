#include "device_swapchain.h"

DeviceSwapchain::DeviceSwapchain(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t graphicsQueueIndex, uint32_t presentQueueIndex) {
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
  
  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
  std::vector<VkSurfaceFormatKHR> surfaceFormatList(formatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormatList.data());

  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
  std::vector<VkPresentModeKHR> presentModeList(presentModeCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModeList.data());

  VkSurfaceFormatKHR surfaceFormat = surfaceFormatList[0];
  VkPresentModeKHR presentMode = presentModeList[0];
  VkExtent2D extent = surfaceCapabilities.currentExtent;

  this->swapchainImageCount = surfaceCapabilities.minImageCount + 1;
  if (surfaceCapabilities.maxImageCount > 0 && this->swapchainImageCount > surfaceCapabilities.maxImageCount) {
    this->swapchainImageCount = surfaceCapabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface = surface;
  swapchainCreateInfo.minImageCount = this->swapchainImageCount;
  swapchainCreateInfo.imageFormat = surfaceFormat.format;
  swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapchainCreateInfo.imageExtent = extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

  if (graphicsQueueIndex != presentQueueIndex) {
    uint32_t queueFamilyIndices[2] = {graphicsQueueIndex, presentQueueIndex};

    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainCreateInfo.queueFamilyIndexCount = 2;
    swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else {
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = presentMode;
  swapchainCreateInfo.clipped = VK_TRUE;
  swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, NULL, &this->swapchain) != VK_SUCCESS) {
    printf("failed to create swapchain\n");
  }

  vkGetSwapchainImagesKHR(logicalDevice, this->swapchain, &this->swapchainImageCount, NULL);
  this->swapchainImageList.resize(this->swapchainImageCount);
  vkGetSwapchainImagesKHR(logicalDevice, this->swapchain, &this->swapchainImageCount, this->swapchainImageList.data());

  this->swapchainImageFormat = surfaceFormat.format;
  this->swapchainExtent = extent;

  this->swapchainImageViewList.resize(this->swapchainImageCount);

  for (int x = 0; x < this->swapchainImageCount; x++) {
    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = this->swapchainImageList[x];
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = this->swapchainImageFormat;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(logicalDevice, &imageViewCreateInfo, NULL, &this->swapchainImageViewList[x]) != VK_SUCCESS) {
      printf("failed to create image view #%d\n", x);
    }
  }
}

DeviceSwapchain::~DeviceSwapchain() {

}

VkFormat DeviceSwapchain::getSwapchainImageFormat() {
  return this->swapchainImageFormat;
}