#include "gui.h"

GUI::GUI(GLFWwindow* window, 
         VkInstance instance, 
         VkPhysicalDevice physicalDevice, 
         VkDevice logicalDevice, 
         uint32_t queueFamily, 
         VkQueue queue, 
         VkDescriptorPool descriptorPool,
         uint32_t minImageCount,
         uint32_t imageCount,
         VkRenderPass renderPass) {

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  ImGui_ImplGlfw_InitForVulkan(window, true);

  ImGui_ImplVulkan_InitInfo initInfo = {
    .Instance = instance,
    .PhysicalDevice = physicalDevice,
    .Device = logicalDevice,
    .QueueFamily = queueFamily,
    .Queue = queue,
    .PipelineCache = VK_NULL_HANDLE,
    .DescriptorPool = descriptorPool,
    .MinImageCount = minImageCount,
    .ImageCount = imageCount,
    .Allocator = NULL,
    .CheckVkResultFn = NULL
  };
  ImGui_ImplVulkan_Init(&initInfo, renderPass);
}

GUI::~GUI() {

}