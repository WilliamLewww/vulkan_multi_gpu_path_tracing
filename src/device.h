#pragma once

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "camera.h"
#include "transformation.h"

class Device {
protected:
  uint32_t framesInFlight;

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

  VkImageView rayTraceImageView;
  VkImage rayTraceImage;
  VkDeviceMemory rayTraceImageMemory;

  VkBuffer cameraUniformBuffer;
  VkDeviceMemory cameraUniformBufferMemory;

  VkBuffer transformUniformBuffer;
  VkDeviceMemory transformUniformBufferMemory;

  std::vector<VkCommandBuffer> commandBufferList;
  std::vector<VkSemaphore> imageAvailableSemaphoreList;
  std::vector<VkSemaphore> renderFinishedSemaphoreList;
  std::vector<VkFence> inFlightFenceList;
  std::vector<VkFence> imageInFlightList;
  uint32_t currentFrame;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();

  VkDevice getLogicalDevice();
  VkExtent2D getSwapchainExtent();
  VkRenderPass getRenderPass();
  VkPhysicalDeviceMemoryProperties getPhysicalMemoryProperties();

  VkBuffer getCameraUniformBuffer();
  VkBuffer getTransformUniformBuffer();
  VkCommandPool getCommandPool();
  VkQueue getComputeQueue();

  VkImageView getRayTraceImageView();

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

  void createTextures();
  void createUniformBuffers(uint32_t instanceCount, std::vector<float> totalTransformBuffer);

  void createCommandBuffers(std::vector<VkBuffer> vertexBufferList, 
                            std::vector<VkBuffer> indexBufferList, 
                            std::vector<uint32_t> primitiveCountList, 
                            VkPipeline pipeline, 
                            VkPipelineLayout pipelineLayout, 
                            std::vector<VkDescriptorSet>& descriptorSetList);

  void createSynchronizationObjects();

  void updateCameraUniformBuffer(CameraUniform camera);
  void drawFrame(CameraUniform camera);
};