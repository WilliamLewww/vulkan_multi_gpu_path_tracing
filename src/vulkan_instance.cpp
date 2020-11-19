#include "vulkan_instance.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
  printf("\033[22;36mvalidation layer\033[0m: \033[22;33m%s\033[0m\n", pCallbackData->pMessage);  

  return VK_FALSE;
}

VulkanInstance::VulkanInstance(const char* applicationName, std::vector<const char*> extensionList, std::vector<const char*> layerList, bool enableValidation) {
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  for (int x = 0; x < glfwExtensionCount; x++) {
    this->extensionList.push_back(glfwExtensions[x]);
  }
  for (int x = 0; x < extensionList.size(); x++) {
    this->extensionList.push_back(extensionList[x]);
  }

  VkApplicationInfo applicationInfo = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = NULL,
    .pApplicationName = applicationName,
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = "No Engine",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_2
  };
  
  VkInstanceCreateInfo instanceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .pApplicationInfo = &applicationInfo,
    .enabledLayerCount = (uint32_t)layerList.size(),
    .ppEnabledLayerNames = layerList.data(),
    .enabledExtensionCount = (uint32_t)this->extensionList.size(),
    .ppEnabledExtensionNames = &this->extensionList[0],
  };

  if (enableValidation) {
    VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = NULL,
      .flags = 0,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debugCallback,
      .pUserData = NULL
    };

    instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&messengerCreateInfo;

    if (vkCreateInstance(&instanceCreateInfo, NULL, &this->instance) != VK_SUCCESS) {
      printf("failed to create Vulkan instance\n");
    }

    PFN_vkCreateDebugUtilsMessengerEXT pvkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance, "vkCreateDebugUtilsMessengerEXT");
    if (pvkCreateDebugUtilsMessengerEXT(this->instance, &messengerCreateInfo, NULL, &this->debugMessenger) != VK_SUCCESS) {
      printf("failed to create debug messenger\n");
    }
  }
  else {
    instanceCreateInfo.pNext = NULL;

    if (vkCreateInstance(&instanceCreateInfo, NULL, &this->instance) != VK_SUCCESS) {
      printf("failed to create Vulkan instance\n");
    }
  }
}

VulkanInstance::~VulkanInstance() {

}

VkInstance VulkanInstance::getVulkanInstance() {
  return this->instance;
}