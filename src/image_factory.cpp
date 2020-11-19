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

  VkImageCreateInfo imageCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .imageType = VK_IMAGE_TYPE_2D,
    .format = format,
    .extent = {
      .width = width,
      .height = height,
      .depth = 1
    },
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = tiling,
    .usage = usageFlags,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = NULL,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
  };

  if (vkCreateImage(logicalDevice, &imageCreateInfo, NULL, image) != VK_SUCCESS) {
    printf("failed to create image\n");
  }

  VkMemoryRequirements memoryRequirements;
  vkGetImageMemoryRequirements(logicalDevice, *image, &memoryRequirements);

  VkMemoryAllocateInfo memoryAllocateInfo = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext = NULL,
    .allocationSize = memoryRequirements.size,
    .memoryTypeIndex = 0
  };
  
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