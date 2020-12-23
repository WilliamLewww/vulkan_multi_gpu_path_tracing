#include "graphics_pipeline.h"

GraphicsPipeline::GraphicsPipeline(std::string vertexShaderFile, std::vector<VkDescriptorSetLayout> descriptorSetLayoutList, VkDevice logicalDevice, VkExtent2D swapchainExtent, VkRenderPass renderPass) {
  FILE* vertexFile = fopen(vertexShaderFile.c_str(), "rb");
  fseek(vertexFile, 0, SEEK_END);
  uint32_t vertexFileSize = ftell(vertexFile);
  fseek(vertexFile, 0, SEEK_SET);

  char* vertexFileBuffer = (char*)malloc(sizeof(char*) * vertexFileSize);
  fread(vertexFileBuffer, 1, vertexFileSize, vertexFile);
  fclose(vertexFile);

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .setLayoutCount = (uint32_t)descriptorSetLayoutList.size(),
    .pSetLayouts = descriptorSetLayoutList.data(),
    .pushConstantRangeCount = 0,
    .pPushConstantRanges = NULL
  };

  if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, NULL, &this->pipelineLayout) != VK_SUCCESS) {
    printf("failed to create pipeline layout\n");
  }

  VkShaderModuleCreateInfo vertexShaderModuleCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .codeSize = vertexFileSize,
    .pCode = (uint32_t*)vertexFileBuffer
  };
  
  VkShaderModule vertexShaderModule;
  if (vkCreateShaderModule(logicalDevice, &vertexShaderModuleCreateInfo, NULL, &vertexShaderModule) != VK_SUCCESS) {
    printf("failed to create vertex shader module\n");
  }

  VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .stage = VK_SHADER_STAGE_VERTEX_BIT,
    .module = vertexShaderModule,
    .pName = "main",
    .pSpecializationInfo = NULL
  };

  VkPipelineShaderStageCreateInfo shaderStages[1] = {vertexShaderStageInfo};

  std::vector<VkVertexInputBindingDescription> vertexBindingDescriptionList(1);
  vertexBindingDescriptionList[0].binding = 0;
  vertexBindingDescriptionList[0].stride = sizeof(float) * 3;
  vertexBindingDescriptionList[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptionList(1);
  vertexAttributeDescriptionList[0].binding = 0;
  vertexAttributeDescriptionList[0].location = 0;
  vertexAttributeDescriptionList[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertexAttributeDescriptionList[0].offset = 0;

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .vertexBindingDescriptionCount = (uint32_t)vertexBindingDescriptionList.size(),
    .pVertexBindingDescriptions = vertexBindingDescriptionList.data(),
    .vertexAttributeDescriptionCount = (uint32_t)vertexAttributeDescriptionList.size(),
    .pVertexAttributeDescriptions = vertexAttributeDescriptionList.data(),
  };
  
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE,
  };

  VkViewport viewport = {
    .x = 0.0f,
    .y = (float)swapchainExtent.height,
    .width = (float)swapchainExtent.width,
    .height = -(float)swapchainExtent.height,
    .minDepth = 0.0f,
    .maxDepth = 1.0f
  };

  VkRect2D scissor = {
    .offset = {
      .x = 0, 
      .y = 0
    },
    .extent = swapchainExtent
  };

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor
  };

  VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .depthBiasConstantFactor = 0,
    .depthBiasClamp = 0,
    .depthBiasSlopeFactor = 0,
    .lineWidth = 1.0f
  };

  VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
    .minSampleShading = 0,
    .pSampleMask = NULL,
    .alphaToCoverageEnable = VK_FALSE,
    .alphaToOneEnable = VK_FALSE
  };

  VkPipelineDepthStencilStateCreateInfo depthStencil = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .depthTestEnable = VK_TRUE,
    .depthWriteEnable = VK_TRUE,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = VK_FALSE,
    .stencilTestEnable = VK_FALSE,
    .front = {},
    .back = {},
    .minDepthBounds = 0,
    .maxDepthBounds = 0
  };

  VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
    .blendEnable = VK_FALSE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .alphaBlendOp = VK_BLEND_OP_ADD,
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
  };

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,
    .pAttachments = &colorBlendAttachmentState,
    .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
  };  

  VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .stageCount = 1,
    .pStages = shaderStages,
    .pVertexInputState = &vertexInputStateCreateInfo,
    .pInputAssemblyState = &inputAssemblyCreateInfo,
    .pTessellationState = NULL,
    .pViewportState = &viewportStateCreateInfo,
    .pRasterizationState = &rasterizationStateCreateInfo,
    .pMultisampleState = &multisampleStateCreateInfo,
    .pDepthStencilState = &depthStencil,
    .pColorBlendState = &colorBlendStateCreateInfo,
    .pDynamicState = NULL,
    .layout = this->pipelineLayout,
    .renderPass = renderPass,
    .subpass = 0,
    .basePipelineHandle = VK_NULL_HANDLE,
    .basePipelineIndex = 0
  };

  if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, NULL, &this->graphicsPipeline) != VK_SUCCESS) {
    printf("failed to create graphics pipeline\n");
  }

  vkDestroyShaderModule(logicalDevice, vertexShaderModule, NULL);

  free(vertexFileBuffer);
}

GraphicsPipeline::GraphicsPipeline(std::string vertexShaderFile, std::string fragmentShaderFile, std::vector<VkDescriptorSetLayout> descriptorSetLayoutList, VkDevice logicalDevice, VkExtent2D swapchainExtent, VkRenderPass renderPass) {
  FILE* vertexFile = fopen(vertexShaderFile.c_str(), "rb");
  fseek(vertexFile, 0, SEEK_END);
  uint32_t vertexFileSize = ftell(vertexFile);
  fseek(vertexFile, 0, SEEK_SET);

  char* vertexFileBuffer = (char*)malloc(sizeof(char*) * vertexFileSize);
  fread(vertexFileBuffer, 1, vertexFileSize, vertexFile);
  fclose(vertexFile);

  FILE* fragmentFile = fopen(fragmentShaderFile.c_str(), "rb");
  fseek(fragmentFile, 0, SEEK_END);
  uint32_t fragmentFileSize = ftell(fragmentFile);
  fseek(fragmentFile, 0, SEEK_SET);

  char* fragmentFileBuffer = (char*)malloc(sizeof(char*) * fragmentFileSize);
  fread(fragmentFileBuffer, 1, fragmentFileSize, fragmentFile);
  fclose(fragmentFile);

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .setLayoutCount = (uint32_t)descriptorSetLayoutList.size(),
    .pSetLayouts = descriptorSetLayoutList.data(),
    .pushConstantRangeCount = 0,
    .pPushConstantRanges = NULL
  };

  if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, NULL, &this->pipelineLayout) != VK_SUCCESS) {
    printf("failed to create pipeline layout\n");
  }

  VkShaderModuleCreateInfo vertexShaderModuleCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .codeSize = vertexFileSize,
    .pCode = (uint32_t*)vertexFileBuffer
  };
  
  VkShaderModule vertexShaderModule;
  if (vkCreateShaderModule(logicalDevice, &vertexShaderModuleCreateInfo, NULL, &vertexShaderModule) != VK_SUCCESS) {
    printf("failed to create vertex shader module\n");
  }

  VkShaderModuleCreateInfo fragmentShaderModuleCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .codeSize = fragmentFileSize,
    .pCode = (uint32_t*)fragmentFileBuffer
  };

  VkShaderModule fragmentShaderModule;
  if (vkCreateShaderModule(logicalDevice, &fragmentShaderModuleCreateInfo, NULL, &fragmentShaderModule) != VK_SUCCESS) {
    printf("failed to create fragment shader module\n");
  }

  VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .stage = VK_SHADER_STAGE_VERTEX_BIT,
    .module = vertexShaderModule,
    .pName = "main",
    .pSpecializationInfo = NULL
  };
  
  VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
    .module = fragmentShaderModule,
    .pName = "main",
    .pSpecializationInfo = NULL 
  };

  VkPipelineShaderStageCreateInfo shaderStages[2] = {vertexShaderStageInfo, fragmentShaderStageInfo};

  std::vector<VkVertexInputBindingDescription> vertexBindingDescriptionList(1);
  vertexBindingDescriptionList[0].binding = 0;
  vertexBindingDescriptionList[0].stride = sizeof(float) * 3;
  vertexBindingDescriptionList[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptionList(1);
  vertexAttributeDescriptionList[0].binding = 0;
  vertexAttributeDescriptionList[0].location = 0;
  vertexAttributeDescriptionList[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertexAttributeDescriptionList[0].offset = 0;

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .vertexBindingDescriptionCount = (uint32_t)vertexBindingDescriptionList.size(),
    .pVertexBindingDescriptions = vertexBindingDescriptionList.data(),
    .vertexAttributeDescriptionCount = (uint32_t)vertexAttributeDescriptionList.size(),
    .pVertexAttributeDescriptions = vertexAttributeDescriptionList.data(),
  };
  
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE,
  };

  VkViewport viewport = {
    .x = 0.0f,
    .y = (float)swapchainExtent.height,
    .width = (float)swapchainExtent.width,
    .height = -(float)swapchainExtent.height,
    .minDepth = 0.0f,
    .maxDepth = 1.0f
  };

  VkRect2D scissor = {
    .offset = {
      .x = 0, 
      .y = 0
    },
    .extent = swapchainExtent
  };

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor
  };

  VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .depthBiasConstantFactor = 0,
    .depthBiasClamp = 0,
    .depthBiasSlopeFactor = 0,
    .lineWidth = 1.0f
  };

  VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
    .minSampleShading = 0,
    .pSampleMask = NULL,
    .alphaToCoverageEnable = VK_FALSE,
    .alphaToOneEnable = VK_FALSE
  };

  VkPipelineDepthStencilStateCreateInfo depthStencil = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .depthTestEnable = VK_TRUE,
    .depthWriteEnable = VK_TRUE,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = VK_FALSE,
    .stencilTestEnable = VK_FALSE,
    .front = {},
    .back = {},
    .minDepthBounds = 0,
    .maxDepthBounds = 0
  };

  VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
    .blendEnable = VK_FALSE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .alphaBlendOp = VK_BLEND_OP_ADD,
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
  };

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,
    .pAttachments = &colorBlendAttachmentState,
    .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
  };  

  VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .stageCount = 2,
    .pStages = shaderStages,
    .pVertexInputState = &vertexInputStateCreateInfo,
    .pInputAssemblyState = &inputAssemblyCreateInfo,
    .pTessellationState = NULL,
    .pViewportState = &viewportStateCreateInfo,
    .pRasterizationState = &rasterizationStateCreateInfo,
    .pMultisampleState = &multisampleStateCreateInfo,
    .pDepthStencilState = &depthStencil,
    .pColorBlendState = &colorBlendStateCreateInfo,
    .pDynamicState = NULL,
    .layout = this->pipelineLayout,
    .renderPass = renderPass,
    .subpass = 1,
    .basePipelineHandle = VK_NULL_HANDLE,
    .basePipelineIndex = 0
  };

  if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, NULL, &this->graphicsPipeline) != VK_SUCCESS) {
    printf("failed to create graphics pipeline\n");
  }

  vkDestroyShaderModule(logicalDevice, vertexShaderModule, NULL);
  vkDestroyShaderModule(logicalDevice, fragmentShaderModule, NULL);

  free(vertexFileBuffer);
  free(fragmentFileBuffer);
}

GraphicsPipeline::~GraphicsPipeline() {

}

VkPipelineLayout GraphicsPipeline::getPipelineLayout() {
  return this->pipelineLayout;
}

VkPipeline GraphicsPipeline::getGraphicsPipeline() {
  return this->graphicsPipeline;
}