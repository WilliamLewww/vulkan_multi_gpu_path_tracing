#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

#include "camera.h"
#include "renderer.h"
#include "model_instance_collection.h"

class GUI {
private:
public:
  GUI(GLFWwindow* window, 
      VkInstance instance, 
      VkPhysicalDevice physicalDevice, 
      VkDevice logicalDevice, 
      uint32_t queueFamily, 
      VkQueue queue, 
      uint32_t minImageCount,
      uint32_t imageCount,
      VkRenderPass renderPass,
      VkCommandPool commandPool);
  
  ~GUI();

  void render(Camera* camera, Renderer* renderer, ModelInstanceCollection* modelInstanceCollection);
};