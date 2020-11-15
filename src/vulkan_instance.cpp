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

  VkApplicationInfo applicationInfo = {};
  applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  applicationInfo.pApplicationName = applicationName;
  applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  applicationInfo.pEngineName = "No Engine";
  applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  applicationInfo.apiVersion = VK_API_VERSION_1_2;
  
  VkInstanceCreateInfo instanceCreateInfo = {};
  instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceCreateInfo.flags = 0;
  instanceCreateInfo.pApplicationInfo = &applicationInfo;
  instanceCreateInfo.enabledExtensionCount = this->extensionList.size();
  instanceCreateInfo.ppEnabledExtensionNames = &this->extensionList[0];
  instanceCreateInfo.enabledLayerCount = layerList.size();

  if (enableValidation) {
    VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {};
    messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    messengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    messengerCreateInfo.pfnUserCallback = debugCallback;

    instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&messengerCreateInfo;
    instanceCreateInfo.ppEnabledLayerNames = &layerList[0];

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

VkInstance VulkanInstance::getInstance() {
  return this->instance;
}