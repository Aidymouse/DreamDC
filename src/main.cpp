
#include <limits>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <util/Files.hpp>

#include <algorithm>
#include <iostream>
#include <optional>
#include <set>
#include <vector>
#include <vulkan/vulkan_core.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

/** TODO: Validation Layers */
// const std::vector<const char *> validation_layers = {
//     "VK_LAYER_KHRONOS_validation"};

/** Queue Families */
struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_qf;
  std::optional<uint32_t> present_qf;

  bool has_all_families() {
    return graphics_qf.has_value() && present_qf.has_value();
  }
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                     VkSurfaceKHR surf) {
  QueueFamilyIndices indicies;

  uint32_t qf_cnt = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &qf_cnt, nullptr);
  std::vector<VkQueueFamilyProperties> qfs(qf_cnt);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &qf_cnt, qfs.data());

  int i = 0;
  for (const auto &qf : qfs) {
    if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indicies.graphics_qf = i;
    }
    VkBool32 supports_present = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surf, &supports_present);
    if (supports_present) {
      indicies.present_qf = i;
    }
    i++;
  }
  return indicies;
}

/** Extensions */
const std::vector<const char *> req_dev_ext = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
bool device_supports_req_extensions(VkPhysicalDevice device) {
  uint32_t ext_cnt;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &ext_cnt, nullptr);
  std::vector<VkExtensionProperties> ready_ext(ext_cnt);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &ext_cnt,
                                       ready_ext.data());

  std::set<std::string> req_ext(req_dev_ext.begin(), req_dev_ext.end());

  for (const auto &ext : ready_ext) {
    req_ext.erase(ext.extensionName);
  }

  return req_ext.empty();
}

/** Swap Chain Support Querying Helpers */
struct Swap_Chain_SD {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;
};

Swap_Chain_SD query_swap_chain_support(VkPhysicalDevice device,
                                       VkSurfaceKHR surface) {
  Swap_Chain_SD details;
  // capabilities
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);
  // formats
  uint32_t format_cnt;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_cnt, nullptr);
  if (format_cnt != 0) {
    details.formats.resize(format_cnt);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_cnt,
                                         details.formats.data());
  }
  // present_modes
  uint32_t pm_cnt;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &pm_cnt, nullptr);
  if (format_cnt != 0) {
    details.present_modes.resize(pm_cnt);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &pm_cnt,
                                              details.present_modes.data());
  }
  return details;
}

VkSurfaceFormatKHR
choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &formats) {
  for (const auto &format : formats) {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }

  return formats[0];
}

VkPresentModeKHR
choose_swap_present_mode(const std::vector<VkPresentModeKHR> &present_modes) {
  // TODO: choose something better if you want
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities,
                              GLFWwindow *window) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D extent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
    };

    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width,
                              capabilities.maxImageExtent.width);
    extent.height =
        std::clamp(extent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);
    return extent;
  }
}

/** Device Validity Checker based on requirements */
bool device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
  QueueFamilyIndices indices = findQueueFamilies(device, surface);

  bool supports_exts = device_supports_req_extensions(device);

  Swap_Chain_SD swap_chain_support = query_swap_chain_support(device, surface);
  bool swap_chain_adequate = !swap_chain_support.formats.empty() &&
                             !swap_chain_support.present_modes.empty();

  if (indices.has_all_families() && supports_exts && swap_chain_adequate) {
    std::cout << "Device with correct queue families + supported extensions + "
                 "swap chain support found"
              << std::endl;
    return true;
  }
  return false;
}

/** Shader Helpers */
VkShaderModule create_shader_module(VkDevice device,
                                    const std::vector<char> &shader_code) {
  VkShaderModuleCreateInfo shader_ci{};
  shader_ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_ci.codeSize = shader_code.size();
  shader_ci.pCode = reinterpret_cast<const uint32_t *>(shader_code.data());
  VkShaderModule module;
  vkCreateShaderModule(device, &shader_ci, nullptr, &module);

  return module;
}

void recordCommand(VkCommandBuffer buffer, uint32_t img_idx,
                   VkRenderPass render_pass, VkFramebuffer *frame_buffers,
                   VkExtent2D extent, VkPipeline graphics_pipeline) {
  VkCommandBufferBeginInfo begin_info;
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  vkBeginCommandBuffer(buffer, &begin_info);

  VkRenderPassBeginInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = render_pass;
  render_pass_info.framebuffer = frame_buffers[img_idx];
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = extent;

  VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  render_pass_info.clearValueCount = 1;
  render_pass_info.pClearValues = &clear_color;

  vkCmdBeginRenderPass(buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

  vkCmdDraw(buffer, 3, 1, 0, 0);
  vkCmdEndRenderPass(buffer);

  if (vkEndCommandBuffer(buffer) != VK_SUCCESS) {
    std::cout << "Error recoridng command buffer" << std::endl;
  }
}

int main() {
  /** Create Window */
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  GLFWwindow *window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                       "It is like being in a dream...", nullptr, nullptr);

  /** Init Vulkan Instance */
  VkInstance main_vk;

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Dream DC";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo inst_create_info{};
  inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  inst_create_info.pApplicationInfo = &app_info;

  // Extensions for instance
  uint32_t glfw_ext_cn = 0;
  const char **glfw_ext;

  glfw_ext = glfwGetRequiredInstanceExtensions(&glfw_ext_cn);

  inst_create_info.enabledExtensionCount = glfw_ext_cn;
  inst_create_info.ppEnabledExtensionNames = glfw_ext;

  std::cout << "GLFW Extensions: " << *glfw_ext << std::endl;

  inst_create_info.enabledLayerCount = 0;

  vkCreateInstance(&inst_create_info, nullptr, &main_vk);

  /** Surface ? */
  VkSurfaceKHR surf;
  VkResult err = glfwCreateWindowSurface(main_vk, window, NULL, &surf);

  /** Find Proper Physical Device */
  VkPhysicalDevice phys_dev = VK_NULL_HANDLE;
  uint32_t dev_cnt = 0;
  vkEnumeratePhysicalDevices(main_vk, &dev_cnt, nullptr);

  std::cout << "Found physical devices: " << dev_cnt << std::endl;

  std::vector<VkPhysicalDevice> devices(dev_cnt);
  vkEnumeratePhysicalDevices(main_vk, &dev_cnt, devices.data());

  for (const auto &dev : devices) {
    if (device_suitable(dev, surf)) {
      phys_dev = dev;
      break;
    }
  }

  if (phys_dev == VK_NULL_HANDLE) {
    throw std::runtime_error("Failed to find suitable GPU");
  }

  /** Logical Device */
  VkDevice log_dev;

  // Queues
  QueueFamilyIndices qf_indicies = findQueueFamilies(phys_dev, surf);
  std::set<uint32_t> uqfs = {qf_indicies.graphics_qf.value(),
                             qf_indicies.present_qf.has_value()};
  std::vector<VkDeviceQueueCreateInfo> q_create_infos;
  float q_priority = 1.0;
  for (uint32_t qf_index : uqfs) {
    VkDeviceQueueCreateInfo q_create_info{};
    q_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    q_create_info.queueFamilyIndex = qf_index;
    q_create_info.queueCount = 1;
    q_create_info.pQueuePriorities = &q_priority;

    q_create_infos.push_back(q_create_info);
  }

  // Features
  VkPhysicalDeviceFeatures dev_features{};

  // Make the damn device
  VkDeviceCreateInfo log_dev_create_info{};
  log_dev_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  log_dev_create_info.pQueueCreateInfos = q_create_infos.data();
  log_dev_create_info.queueCreateInfoCount = 1;
  log_dev_create_info.pEnabledFeatures = &dev_features;
  // Enabled extensions
  log_dev_create_info.enabledExtensionCount =
      static_cast<uint32_t>(req_dev_ext.size());
  log_dev_create_info.ppEnabledExtensionNames = req_dev_ext.data();

  vkCreateDevice(phys_dev, &log_dev_create_info, nullptr, &log_dev);
  std::cout << "Logical device created" << std::endl;

  // Get queue handles
  VkQueue graphics_q;
  VkQueue present_q;

  vkGetDeviceQueue(log_dev, qf_indicies.graphics_qf.value(), 0, &graphics_q);
  vkGetDeviceQueue(log_dev, qf_indicies.present_qf.value(), 0, &present_q);

  /** Swap Chain */
  // Query Swap Chain Support
  Swap_Chain_SD support_details = query_swap_chain_support(phys_dev, surf);
  VkSurfaceFormatKHR chosen_format =
      choose_swap_surface_format(support_details.formats);
  VkPresentModeKHR chosen_present_mode =
      choose_swap_present_mode(support_details.present_modes);
  VkExtent2D extent = choose_swap_extent(support_details.capabilities, window);

  // Get image count
  uint32_t swap_chain_image_count =
      support_details.capabilities.minImageCount + 1;
  if (support_details.capabilities.maxImageCount > 0 &&
      swap_chain_image_count > support_details.capabilities.maxImageCount) {
    swap_chain_image_count = support_details.capabilities.maxImageCount;
  };

  // Make the actual swap chain
  VkSwapchainCreateInfoKHR swapchain_create_info{};
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.surface = surf;
  swapchain_create_info.minImageCount = swap_chain_image_count;
  swapchain_create_info.imageFormat = chosen_format.format;
  swapchain_create_info.imageColorSpace = chosen_format.colorSpace;
  swapchain_create_info.imageExtent = extent;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t qf_indicies_num[] = {qf_indicies.graphics_qf.value(),
                                qf_indicies.present_qf.value()};

  if (qf_indicies.graphics_qf != qf_indicies.present_qf) {
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchain_create_info.queueFamilyIndexCount = 2;
    swapchain_create_info.pQueueFamilyIndices = qf_indicies_num;
  } else {
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = nullptr;
  }

  swapchain_create_info.preTransform =
      support_details.capabilities.currentTransform;
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode = chosen_present_mode;
  swapchain_create_info.clipped = VK_TRUE;
  swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

  VkSwapchainKHR swap_chain;
  vkCreateSwapchainKHR(log_dev, &swapchain_create_info, nullptr, &swap_chain);

  std::cout << "Created swap chain" << std::endl;

  // Get Image Handles
  std::vector<VkImage> swap_chain_images;
  uint32_t image_cnt;
  vkGetSwapchainImagesKHR(log_dev, swap_chain, &image_cnt, nullptr);
  swap_chain_images.resize(image_cnt);
  vkGetSwapchainImagesKHR(log_dev, swap_chain, &image_cnt,
                          swap_chain_images.data());

  // Store some stuff
  VkFormat swap_chain_image_format = chosen_format.format;
  VkExtent2D swap_chain_extent = extent;

  /** Image Views */
  std::vector<VkImageView> sc_img_views;
  sc_img_views.resize(swap_chain_images.size());
  for (size_t i = 0; i < swap_chain_images.size(); i++) {
    VkImageViewCreateInfo img_view_ci{};
    img_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    img_view_ci.image = swap_chain_images[i];
    img_view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    img_view_ci.format = swap_chain_image_format;

    img_view_ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    img_view_ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    img_view_ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    img_view_ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    img_view_ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    img_view_ci.subresourceRange.baseMipLevel = 0;
    img_view_ci.subresourceRange.levelCount = 1;
    img_view_ci.subresourceRange.baseArrayLayer = 0;
    img_view_ci.subresourceRange.layerCount = 1;

    vkCreateImageView(log_dev, &img_view_ci, nullptr, &sc_img_views[i]);
  }
  std::cout << "Created image views" << std::endl;

  /** Make Render Pass */

  // Subpass dependency
  VkSubpassDependency dep{};
  dep.srcSubpass = VK_SUBPASS_EXTERNAL;
  dep.dstSubpass = 0;
  dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.srcAccessMask = 0;
  dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription color_attachment{};
  color_attachment.format = swap_chain_image_format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attach_ref{};
  color_attach_ref.attachment = 0;
  color_attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attach_ref;

  VkRenderPass render_pass;
  VkRenderPassCreateInfo render_pass_ci{};
  render_pass_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_ci.attachmentCount = 1;
  render_pass_ci.pAttachments = &color_attachment;
  render_pass_ci.subpassCount = 1;
  render_pass_ci.pSubpasses = &subpass;
  // render_pass_ci.dependencyCount = 1;
  // render_pass_ci.pDependencies = &dep;

  vkCreateRenderPass(log_dev, &render_pass_ci, nullptr, &render_pass);

  std::cout << "Created Render Pass" << std::endl;

  /** Make the graphics pipeline */
  auto vert_code = readFile("../shaders/vert.spv");
  VkShaderModule vert_module = create_shader_module(log_dev, vert_code);
  auto frag_code = readFile("../shaders/frag.spv");
  VkShaderModule frag_module = create_shader_module(log_dev, frag_code);

  // Shader Stage
  VkPipelineShaderStageCreateInfo vert_shader_ci{};
  vert_shader_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_ci.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_ci.module = vert_module;
  vert_shader_ci.pName = "main";

  VkPipelineShaderStageCreateInfo frag_shader_ci{};
  frag_shader_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_ci.module = frag_module;
  frag_shader_ci.pName = "main";

  VkPipelineShaderStageCreateInfo shade_stages_cis[] = {vert_shader_ci,
                                                        frag_shader_ci};

  // Vertex Stage
  VkPipelineVertexInputStateCreateInfo vertex_input_ci;
  vertex_input_ci.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_ci.vertexBindingDescriptionCount = 0;
  vertex_input_ci.pVertexBindingDescriptions = nullptr;
  vertex_input_ci.vertexAttributeDescriptionCount = 0;
  vertex_input_ci.pVertexAttributeDescriptions = nullptr;

  // Dynamic State
  VkPipelineDynamicStateCreateInfo dynamic_ci{};
  dynamic_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_ci.dynamicStateCount = 0;
  dynamic_ci.pDynamicStates = nullptr;

  VkDynamicState dynamic;

  // Input Assembly
  VkPipelineInputAssemblyStateCreateInfo input_assembly_ci{};
  input_assembly_ci.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_ci.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_ci.primitiveRestartEnable = VK_FALSE;

  // Viewport
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swap_chain_extent.width;
  viewport.height = (float)swap_chain_extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swap_chain_extent;

  VkPipelineViewportStateCreateInfo viewport_state_ci{};
  viewport_state_ci.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_ci.viewportCount = 1;
  viewport_state_ci.pViewports = &viewport;
  viewport_state_ci.scissorCount = 1;
  viewport_state_ci.pScissors = &scissor;

  // Rasterizer
  VkPipelineRasterizationStateCreateInfo rasterizer_ci{};
  rasterizer_ci.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer_ci.depthClampEnable = VK_FALSE;
  rasterizer_ci.rasterizerDiscardEnable = VK_FALSE;
  rasterizer_ci.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer_ci.lineWidth = 1.0f;
  rasterizer_ci.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer_ci.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer_ci.depthBiasEnable = VK_FALSE;
  rasterizer_ci.depthBiasConstantFactor = 0.0f;
  rasterizer_ci.depthBiasClamp = 0.0f;
  rasterizer_ci.depthBiasSlopeFactor = 0.0f;

  VkPipelineMultisampleStateCreateInfo multisampling_ci{};
  multisampling_ci.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_ci.sampleShadingEnable = VK_FALSE;
  multisampling_ci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling_ci.minSampleShading = 1.0f;          // Optional
  multisampling_ci.pSampleMask = nullptr;            // Optional
  multisampling_ci.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling_ci.alphaToOneEnable = VK_FALSE;      // Optional

  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  // Configured to enable alpha blending
  color_blend_attachment.blendEnable = VK_TRUE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo color_blend_ci{};
  color_blend_ci.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_ci.logicOpEnable = VK_FALSE;
  color_blend_ci.logicOp = VK_LOGIC_OP_COPY;
  color_blend_ci.attachmentCount = 1;
  color_blend_ci.pAttachments = &color_blend_attachment;
  color_blend_ci.blendConstants[0] = 0.0f;
  color_blend_ci.blendConstants[1] = 0.0f;
  color_blend_ci.blendConstants[2] = 0.0f;
  color_blend_ci.blendConstants[3] = 0.0f;

  // Pipeline Layout (empty rn)
  VkPipelineLayout pipeline_layout;
  VkPipelineLayoutCreateInfo pipeline_layout_ci{};
  pipeline_layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_ci.setLayoutCount = 0;
  pipeline_layout_ci.pSetLayouts = nullptr;
  pipeline_layout_ci.pushConstantRangeCount = 0;
  pipeline_layout_ci.pPushConstantRanges = nullptr;
  vkCreatePipelineLayout(log_dev, &pipeline_layout_ci, nullptr,
                         &pipeline_layout);

  VkGraphicsPipelineCreateInfo pipeline_ci{};
  pipeline_ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_ci.stageCount = 2;
  pipeline_ci.pStages = shade_stages_cis;
  pipeline_ci.pVertexInputState = &vertex_input_ci;
  pipeline_ci.pInputAssemblyState = &input_assembly_ci;
  pipeline_ci.pViewportState = &viewport_state_ci;
  pipeline_ci.pRasterizationState = &rasterizer_ci;
  pipeline_ci.pMultisampleState = &multisampling_ci;
  pipeline_ci.pDepthStencilState = nullptr;
  pipeline_ci.pColorBlendState = &color_blend_ci;
  pipeline_ci.pDynamicState = &dynamic_ci;
  pipeline_ci.layout = pipeline_layout;
  pipeline_ci.renderPass = render_pass;
  pipeline_ci.subpass = 0;

  VkPipeline graphics_pipeline;
  if (vkCreateGraphicsPipelines(log_dev, VK_NULL_HANDLE, 1, &pipeline_ci,
                                nullptr, &graphics_pipeline) != VK_SUCCESS) {
    std::cout << "Fail";
  };

  std::cout << "Created Pipeline" << std::endl;

  /** Frame Buffers */
  std::vector<VkFramebuffer> sc_frame_buffers;
  sc_frame_buffers.resize(sc_img_views.size());
  for (size_t i = 0; i < sc_img_views.size(); i++) {
    VkImageView attachments[] = {sc_img_views[i]};

    VkFramebufferCreateInfo buffer_ci{};
    buffer_ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    buffer_ci.renderPass = render_pass;
    buffer_ci.attachmentCount = 1;
    buffer_ci.pAttachments = attachments;
    buffer_ci.width = swap_chain_extent.width;
    buffer_ci.height = swap_chain_extent.height;
    buffer_ci.layers = 1;

    vkCreateFramebuffer(log_dev, &buffer_ci, nullptr, &sc_frame_buffers[i]);
  }

  /** Do some fucking drawing!!! */
  VkCommandPool command_pool;
  VkCommandPoolCreateInfo command_pool_ci{};
  command_pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_ci.queueFamilyIndex = qf_indicies.graphics_qf.value();

  vkCreateCommandPool(log_dev, &command_pool_ci, nullptr, &command_pool);
  std::cout << "Created Command Pool" << std::endl;

  VkCommandBuffer command_buffer;
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = 1;

  vkAllocateCommandBuffers(log_dev, &alloc_info, &command_buffer);
  std::cout << "Allocated Command Buffer" << std::endl;

  /** Create Sync Objects */
  VkSemaphore img_available_smph;
  VkSemaphore render_finished_smph;
  VkFence in_flight_fnce;

  VkSemaphoreCreateInfo smph_ci{};
  smph_ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  vkCreateSemaphore(log_dev, &smph_ci, nullptr, &img_available_smph);
  vkCreateSemaphore(log_dev, &smph_ci, nullptr, &render_finished_smph);

  VkFenceCreateInfo fnce_ci{};
  fnce_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fnce_ci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  vkCreateFence(log_dev, &fnce_ci, nullptr, &in_flight_fnce);

  std::cout << "Created Semaphores and Fences" << std::endl;

  /** Window loop */
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    /** Draw Frame */
    vkWaitForFences(log_dev, 1, &in_flight_fnce, VK_TRUE, UINT64_MAX);
    vkResetFences(log_dev, 1, &in_flight_fnce);

    uint32_t img_idx;
    vkAcquireNextImageKHR(log_dev, swap_chain, UINT64_MAX, img_available_smph,
                          VK_NULL_HANDLE, &img_idx);

    vkResetCommandBuffer(command_buffer, 0);

    recordCommand(command_buffer, 0, render_pass, sc_frame_buffers.data(),
                  swap_chain_extent, graphics_pipeline);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_smph[] = {img_available_smph};
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_smph;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    VkSemaphore signal_smph[] = {render_finished_smph};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_smph;

    vkQueueSubmit(graphics_q, 1, &submit_info, in_flight_fnce);

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_smph;

    VkSwapchainKHR swap_chains[] = {swap_chain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &img_idx;
    present_info.pResults = nullptr;

    vkQueuePresentKHR(present_q, &present_info);
  }

  vkDeviceWaitIdle(log_dev);

  /** Clean Up */
  std::cout << "Removing Command Pool" << std::endl;
  vkDestroySemaphore(log_dev, img_available_smph, nullptr);
  vkDestroySemaphore(log_dev, render_finished_smph, nullptr);
  vkDestroyFence(log_dev, in_flight_fnce, nullptr);

  std::cout << "Removing Command Pool" << std::endl;
  vkDestroyCommandPool(log_dev, command_pool, nullptr);

  std::cout << "Removing Buffers" << std::endl;
  for (auto buffer : sc_frame_buffers) {
    vkDestroyFramebuffer(log_dev, buffer, nullptr);
  }

  std::cout << "Removing Pipeline" << std::endl;
  vkDestroyPipeline(log_dev, graphics_pipeline, nullptr);

  std::cout << "Removing Pipeline Layout" << std::endl;
  vkDestroyPipelineLayout(log_dev, pipeline_layout, nullptr);

  std::cout << "Removing Render pass" << std::endl;
  vkDestroyRenderPass(log_dev, render_pass, nullptr);

  std::cout << "Removing Image Views" << std::endl;
  for (auto image_view : sc_img_views) {
    vkDestroyImageView(log_dev, image_view, nullptr);
  }
  std::cout << "Removing Swap Chain" << std::endl;
  vkDestroySwapchainKHR(log_dev, swap_chain, nullptr);

  vkDestroyShaderModule(log_dev, vert_module, nullptr);
  vkDestroyShaderModule(log_dev, frag_module, nullptr);

  std::cout << "Removing Device" << std::endl;
  vkDestroyDevice(log_dev, nullptr);

  std::cout << "Removing Surface" << std::endl;
  vkDestroySurfaceKHR(main_vk, surf, nullptr);
  std::cout << "Removing Instance" << std::endl;
  vkDestroyInstance(main_vk, nullptr);
  std::cout << "Removing Window" << std::endl;
  glfwDestroyWindow(window);

  glfwTerminate();

  return 0;
}
