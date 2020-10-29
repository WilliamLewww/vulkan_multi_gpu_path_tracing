#pragma once

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdio.h>
#include <string.h>

#include "scene.h"

class Device {
protected:
  VkPhysicalDevice physicalDevice;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  VkPhysicalDeviceFeatures physicalDeviceFeatures;
  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

  uint32_t graphicsQueueIndex;
  uint32_t presentQueueIndex;
  uint32_t computeQueueIndex;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VkQueue computeQueue;

  VkDevice logicalDevice;
  VkCommandPool commandPool;

  uint32_t swapchainImageCount;
  VkSwapchainKHR swapchain;
  std::vector<VkImage> swapchainImageList;
  VkFormat swapchainImageFormat;
  VkExtent2D swapchainExtent;
  std::vector<VkImageView> swapchainImageViewList;

  VkRenderPass renderPass;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  std::vector<VkFramebuffer> framebufferList;

  VkBuffer vertexPositionBuffer;
  VkDeviceMemory vertexPositionBufferMemory;

  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  VkBuffer materialIndexBuffer;
  VkDeviceMemory materialIndexBufferMemory;

  VkBuffer materialBuffer;
  VkDeviceMemory materialBufferMemory;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();

  void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage* image, VkDeviceMemory* imageMemory);
  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  void initializeQueues(VkSurfaceKHR surface);
  void createLogicalDevice(std::vector<const char*> extensions);
  void createCommandPool();
  void createSwapchain(VkSurfaceKHR surface);
  void createRenderPass();
  void createDepthResource();
  void createFramebuffers();

  void createVertexBuffer(Scene* scene);
  void createIndexBuffer(Scene* scene);
  void createMaterialBuffers(Scene* scene);
};