#pragma once

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "scene.h"
#include "camera.h"

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

  VkBuffer vertexPositionBuffer;
  VkDeviceMemory vertexPositionBufferMemory;

  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  VkBuffer materialIndexBuffer;
  VkDeviceMemory materialIndexBufferMemory;

  VkBuffer materialBuffer;
  VkDeviceMemory materialBufferMemory;

  VkBuffer materialLightBuffer;
  VkDeviceMemory materialLightBufferMemory;

  VkImageView rayTraceImageView;
  VkImage rayTraceImage;
  VkDeviceMemory rayTraceImageMemory;

  VkAccelerationStructureKHR accelerationStructure;
  VkBuffer accelerationStructureBuffer;
  VkDeviceMemory accelerationStructureBufferMemory;

  VkAccelerationStructureKHR topLevelAccelerationStructure;
  VkBuffer topLevelAccelerationStructureBuffer;
  VkDeviceMemory topLevelAccelerationStructureBufferMemory;

  VkBuffer uniformBuffer;
  VkDeviceMemory uniformBufferMemory;

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

  VkAccelerationStructureKHR* getTopLevelAccelerationStructurePointer();
  VkBuffer getUniformBuffer();
  VkBuffer getIndexBuffer();
  VkBuffer getVertexBuffer();
  VkImageView getRayTraceImageView();
  VkBuffer getMaterialIndexBuffer();
  VkBuffer getMaterialBuffer();
  VkBuffer getMaterialLightBuffer();

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
  void createTextures();

  void createAccelerationStructure(Scene* scene);
  void bindAccelerationStructure();
  void buildAccelerationStructure(Scene* scene);

  void createTopLevelAccelerationStructure();

  void createUniformBuffer();

  void createCommandBuffers(Scene* scene, VkPipeline pipeline, VkPipelineLayout pipelineLayout, std::vector<VkDescriptorSet>& descriptorSetList);
  void createSynchronizationObjects();

  void updateUniformBuffer(CameraUniform camera);
  void drawFrame(CameraUniform camera);
};