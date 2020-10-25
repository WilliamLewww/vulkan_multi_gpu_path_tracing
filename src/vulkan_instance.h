#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdio.h>

class VulkanInstance {
private:
  VkInstance instance;
  std::vector<const char*> extensionList;

  VkDebugUtilsMessengerEXT debugMessenger;
public:
  VulkanInstance(const char* applicationName, std::vector<const char*> extensionList, std::vector<const char*> layerList, bool enableValidation);
  ~VulkanInstance();

  VkInstance getInstance();
};