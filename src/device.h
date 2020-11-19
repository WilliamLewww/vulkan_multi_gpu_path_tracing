#pragma once
#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>
#include <map>
#include <stdio.h>

#include "device_queue.h"
#include "device_command_pool.h"
#include "device_swapchain.h"
#include "device_render_pass.h"
#include "device_depth_resource.h"
#include "device_framebuffers.h"
#include "device_textures.h"

class Device {
private:
  VkPhysicalDevice physicalDevice;
  VkPhysicalDeviceProperties physicalDeviceProperties;
  VkPhysicalDeviceFeatures physicalDeviceFeatures;
  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

  VkDevice logicalDevice;

  DeviceQueue* deviceQueue;
  DeviceCommandPool* deviceCommandPool;
  DeviceSwapchain* deviceSwapchain;
  DeviceRenderPass* deviceRenderPass;
  DeviceDepthResource* deviceDepthResource;
  DeviceFramebuffers* deviceFramebuffers;
  DeviceTextures* deviceTextures;
public:
  Device(VkPhysicalDevice physicalDevice);
  ~Device();

  void initializeDeviceQueue(VkSurfaceKHR surface);
  void createLogicalDevice(std::vector<const char*> extensions);
  void createCommandPool();
  void createSwapchain(VkSurfaceKHR surface);
  void createRenderPass();
  void createDepthResource();
  void createFramebuffers();
  void createTextures();
};