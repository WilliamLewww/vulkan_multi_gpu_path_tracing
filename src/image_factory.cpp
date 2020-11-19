#include "image_factory.h"

void ImageFactory::createImage(VkDevice logicalDevice,
                               VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties,
                               uint32_t width, 
                               uint32_t height, 
                               VkFormat format, 
                               VkImageTiling tiling, 
                               VkImageUsageFlags usageFlags, 
                               VkMemoryPropertyFlags propertyFlags, 
                               VkImage* image, 
                               VkDeviceMemory* imageMemory) {

  VkImageCreateInfo imageCreateInfo = {};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.extent.width = width;
  imageCreateInfo.extent.height = height;
  imageCreateInfo.extent.depth = 1;
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.format = format;
  imageCreateInfo.tiling = tiling;
  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageCreateInfo.usage = usageFlags;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(logicalDevice, &imageCreateInfo, NULL, image) != VK_SUCCESS) {
    printf("failed to create image\n");
  }

  VkMemoryRequirements memoryRequirements;
  vkGetImageMemoryRequirements(logicalDevice, *image, &memoryRequirements);

  VkMemoryAllocateInfo memoryAllocateInfo = {};
  memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memoryAllocateInfo.allocationSize = memoryRequirements.size;
  
  uint32_t memoryTypeIndex = -1;
  for (int x = 0; x < physicalDeviceMemoryProperties.memoryTypeCount; x++) {
    if ((memoryRequirements.memoryTypeBits & (1 << x)) && (physicalDeviceMemoryProperties.memoryTypes[x].propertyFlags & propertyFlags) == propertyFlags) {
      memoryTypeIndex = x;
      break;
    }
  }
  memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

  if (vkAllocateMemory(logicalDevice, &memoryAllocateInfo, NULL, imageMemory) != VK_SUCCESS) {
    printf("failed to allocate image memory\n");
  }

  vkBindImageMemory(logicalDevice, *image, *imageMemory, 0);
}