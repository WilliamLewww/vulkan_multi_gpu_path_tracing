#include "graphics_pipeline.h"

GraphicsPipeline::GraphicsPipeline() {
  this->vertexFileBuffer = NULL;
  this->fragmentFileBuffer = NULL;
}

GraphicsPipeline::~GraphicsPipeline() {
  if (vertexFileBuffer) {
    free(vertexFileBuffer);
  }
  if (fragmentFileBuffer) {
    free(fragmentFileBuffer);
  }
}

VkPipelineLayout GraphicsPipeline::getPipelineLayout() {
  return this->pipelineLayout;
}

VkPipeline GraphicsPipeline::getPipeline() {
  return this->graphicsPipeline;
}

void GraphicsPipeline::setVertexFile(std::string path) {
  this->vertexFile = fopen(path.c_str(), "rb");
  fseek(this->vertexFile, 0, SEEK_END);
  this->vertexFileSize = ftell(this->vertexFile);
  fseek(this->vertexFile, 0, SEEK_SET);

  if (vertexFileBuffer) {
    free(vertexFileBuffer);
  }
  this->vertexFileBuffer = (char*)malloc(sizeof(char*) * this->vertexFileSize);
  fread(this->vertexFileBuffer, 1, this->vertexFileSize, this->vertexFile);
  fclose(this->vertexFile);
}

void GraphicsPipeline::setFragmentFile(std::string path) {
  this->fragmentFile = fopen(path.c_str(), "rb");
  fseek(this->fragmentFile, 0, SEEK_END);
  this->fragmentFileSize = ftell(this->fragmentFile);
  fseek(this->fragmentFile, 0, SEEK_SET);

  if (fragmentFileBuffer) {
    free(fragmentFileBuffer);
  }
  this->fragmentFileBuffer = (char*)malloc(sizeof(char*) * this->fragmentFileSize);
  fread(this->fragmentFileBuffer, 1, this->fragmentFileSize, this->fragmentFile);
  fclose(this->fragmentFile);
}

void GraphicsPipeline::createPipelineLayout(VkDevice logicalDevice, std::vector<VkDescriptorSetLayout> descriptorSetLayoutList) {
  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayoutList.size();
  pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayoutList.data();

  if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, NULL, &this->pipelineLayout) == VK_SUCCESS) {
    printf("created pipeline layout\n");
  } 
}

void GraphicsPipeline::createGraphicsPipeline(VkDevice logicalDevice,
                                              std::vector<VkVertexInputBindingDescription> vertexBindingDescriptionList,
                                              std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptionList,
                                              VkExtent2D swapchainExtent,
                                              VkRenderPass renderPass) {

  VkShaderModuleCreateInfo vertexShaderModuleCreateInfo = {};
  vertexShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  vertexShaderModuleCreateInfo.codeSize = this->vertexFileSize;
  vertexShaderModuleCreateInfo.pCode = (uint32_t*)this->vertexFileBuffer;
  
  VkShaderModule vertexShaderModule;
  if (vkCreateShaderModule(logicalDevice, &vertexShaderModuleCreateInfo, NULL, &vertexShaderModule) == VK_SUCCESS) {
    printf("created vertex shader module\n");
  }

  VkShaderModuleCreateInfo fragmentShaderModuleCreateInfo = {};
  fragmentShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  fragmentShaderModuleCreateInfo.codeSize = this->fragmentFileSize;
  fragmentShaderModuleCreateInfo.pCode = (uint32_t*)this->fragmentFileBuffer;

  VkShaderModule fragmentShaderModule;
  if (vkCreateShaderModule(logicalDevice, &fragmentShaderModuleCreateInfo, NULL, &fragmentShaderModule) == VK_SUCCESS) {
    printf("created fragment shader module\n");
  }

  VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {};
  vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexShaderStageInfo.module = vertexShaderModule;
  vertexShaderStageInfo.pName = "main";
  
  VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {};
  fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStageInfo.module = fragmentShaderModule;
  fragmentShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[2] = {vertexShaderStageInfo, fragmentShaderStageInfo};

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
  vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount = vertexBindingDescriptionList.size();
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptionList.size();
  vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindingDescriptionList.data();
  vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptionList.data();
  
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
  inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = (float)swapchainExtent.height;
  viewport.width = (float)swapchainExtent.width;
  viewport.height = -(float)swapchainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  VkOffset2D scissorOffset = {0, 0};
  scissor.offset = scissorOffset;
  scissor.extent = swapchainExtent;

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
  viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.pViewports = &viewport;
  viewportStateCreateInfo.scissorCount = 1;
  viewportStateCreateInfo.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
  rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
  rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
  rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationStateCreateInfo.lineWidth = 1.0f;
  rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
  multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
  multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
  colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachmentState.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};  
  colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendStateCreateInfo.attachmentCount = 1;
  colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
  colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
  colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
  colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
  colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

  VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
  graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphicsPipelineCreateInfo.stageCount = 2;
  graphicsPipelineCreateInfo.pStages = shaderStages;
  graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
  graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
  graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
  graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
  graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
  graphicsPipelineCreateInfo.pDepthStencilState = &depthStencil;
  graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
  graphicsPipelineCreateInfo.layout = this->pipelineLayout;
  graphicsPipelineCreateInfo.renderPass = renderPass;
  graphicsPipelineCreateInfo.subpass = 0;
  graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;  

  if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, NULL, &this->graphicsPipeline) == VK_SUCCESS) {
    printf("created graphics pipeline\n");
  }

  vkDestroyShaderModule(logicalDevice, vertexShaderModule, NULL);
  vkDestroyShaderModule(logicalDevice, fragmentShaderModule, NULL);
}