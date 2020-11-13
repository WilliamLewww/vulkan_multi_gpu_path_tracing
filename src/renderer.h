#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "input.h"
#include "window.h"
#include "vulkan_instance.h"
#include "device_manager.h"
#include "camera.h"
#include "graphics_pipeline.h"
#include "descriptor_manager.h"
#include "acceleration_structure_manager.h"
#include "model.h"
#include "instance_manager.h"

class Renderer {
private:
  Window* window;
  VulkanInstance* vulkanInstance;
  DeviceManager* deviceManager;
  InstanceManager* instanceManager;
  AccelerationStructureManager* accelerationStructureManager;
  DescriptorManager* descriptorManager;
  GraphicsPipeline* graphicsPipeline;
public:
  Renderer(std::vector<Model*> modelList, Camera* camera);
  ~Renderer();
};