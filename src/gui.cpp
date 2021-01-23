#include "gui.h"

GUI::GUI(GLFWwindow* window, 
         VkInstance instance, 
         VkPhysicalDevice physicalDevice, 
         VkDevice logicalDevice, 
         uint32_t queueFamily, 
         VkQueue queue, 
         uint32_t minImageCount,
         uint32_t imageCount,
         VkRenderPass renderPass,
         VkCommandPool commandPool) {

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  ImGui_ImplGlfw_InitForVulkan(window, true);

  VkDescriptorPool descriptorPool;

  VkDescriptorPoolSize pool_sizes[] = {
      { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
      { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
      { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
      { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
  };
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
  pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;
  vkCreateDescriptorPool(logicalDevice, &pool_info, NULL, &descriptorPool);

  ImGui_ImplVulkan_InitInfo initInfo = {
    .Instance = instance,
    .PhysicalDevice = physicalDevice,
    .Device = logicalDevice,
    .QueueFamily = queueFamily,
    .Queue = queue,
    .PipelineCache = VK_NULL_HANDLE,
    .DescriptorPool = descriptorPool,
    .Subpass = 4,
    .MinImageCount = minImageCount,
    .ImageCount = imageCount,
    .Allocator = NULL,
    .CheckVkResultFn = NULL
  };
  ImGui_ImplVulkan_Init(&initInfo, renderPass);

  VkCommandBufferAllocateInfo bufferAllocateInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = NULL,
    .commandPool = commandPool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1
  };

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(logicalDevice, &bufferAllocateInfo, &commandBuffer);
  
  VkCommandBufferBeginInfo commandBufferBeginInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = NULL,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    .pInheritanceInfo = NULL
  };
  
  vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
  ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = NULL,
    .waitSemaphoreCount = 0,
    .pWaitSemaphores = NULL,
    .pWaitDstStageMask = NULL,
    .commandBufferCount = 1,
    .pCommandBuffers = &commandBuffer,
    .signalSemaphoreCount = 0,
    .pSignalSemaphores = NULL
  };

  vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);

  vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}

GUI::~GUI() {

}

void GUI::render(Camera* camera, Camera* minimapCamera, Renderer* renderer, ModelInstanceSetCollection* modelInstanceSetCollection) {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowSize(ImVec2(250, 300));
  ImGui::Begin("Dashboard", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
  ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
  ImGui::Text("Press \"Q\" to toggle cursor");

  ImGui::PushID("#WORLDCAMERA");
  if (ImGui::CollapsingHeader("World Camera Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::DragFloat3("Position", camera->getPosition(), 0.01, 0.0, 0.0, "%.2f");
  }
  ImGui::PopID();

  ImGui::PushID("#MINIMAPCAMERA");
  if (ImGui::CollapsingHeader("Minimap Camera Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::DragFloat("Radius", minimapCamera->getPosition(), 0.01, 0.0, 0.0, "%.2f");
    ImGui::DragFloat("Pitch", minimapCamera->getPitch(), 0.01, 0.0, 0.0, "%.2f");
    ImGui::DragFloat("Yaw", minimapCamera->getYaw(), 0.01, 0.0, 0.0, "%.2f");
  }
  ImGui::PopID();

  if (ImGui::CollapsingHeader("Model Collections")) {
    for (int y = 0; y < modelInstanceSetCollection->getModelInstanceSetCount(); y++) {
      std::string title2 = "  Collection #" + std::to_string(y);
      std::string label2 = "#COLLECTION" + std::to_string(y);
      ImGui::PushID(label2.c_str());

      if (ImGui::CollapsingHeader(title2.c_str())) {
        for (int x = 0; x < modelInstanceSetCollection->getModelInstanceSet(y)->getInstanceCount(); x++) {
          std::string title = "    " + modelInstanceSetCollection->getModelInstanceSet(y)->getModelInstance(x)->getModel()->getFileName();
          std::string label = "#INSTANCE" + std::to_string(x);
          ImGui::PushID(label.c_str());
          if (ImGui::CollapsingHeader(title.c_str())) {
            if (ImGui::DragFloat3("Position", modelInstanceSetCollection->getModelInstanceSet(y)->getModelInstance(x)->getTransformation().getPosition(), 0.01, 0.0, 0.0, "%.2f")) {
              modelInstanceSetCollection->getModelInstanceSet(y)->getModelInstance(x)->getTransformation().updateTransformation();
              modelInstanceSetCollection->getModelInstanceSet(y)->updateUniformBuffer();
              renderer->updateModelInstancesUniformBuffers(y + 2, y);
              renderer->updateAccelerationStructure(y);
              camera->resetFrames();
            }
            if (ImGui::DragFloat3("Scale", modelInstanceSetCollection->getModelInstanceSet(y)->getModelInstance(x)->getTransformation().getScale(), 0.01, 0.0, 0.0, "%.2f")) {
              modelInstanceSetCollection->getModelInstanceSet(y)->getModelInstance(x)->getTransformation().updateTransformation();
              modelInstanceSetCollection->getModelInstanceSet(y)->updateUniformBuffer();
              renderer->updateModelInstancesUniformBuffers(y + 2, y);
              renderer->updateAccelerationStructure(y);
              camera->resetFrames();
            }
          }
          ImGui::PopID();
        }
      }

      ImGui::PopID();
    }
  }

  ImGui::End();

  ImGui::Render();
}