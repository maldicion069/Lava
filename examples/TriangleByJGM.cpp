/**
 * Lava.
 * File: TriangleByJGM.cpp
 * Author: Juan Guerrero Martín.
 * Brief: Following The Khronos Group Inc. tutorial (https://vulkan-tutorial.com/).
 */

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstring>
#include <functional>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

/** GLFW. **/
const int WIDTH = 800;
const int HEIGHT = 600;

/** Shaders. They must be in *.spv Vulkan format. **/
std::string triangleVS( "/home/jguerrero/opt/Lava/resources/shaders/sh/triangleByJGM_vert.spv" );
std::string triangleFS( "/home/jguerrero/opt/Lava/resources/shaders/sh/triangleByJGM_frag.spv" );

/** Instance-related. **/

// Using validation layers only if debugging.
const std::vector< const char* > validationLayers =
{
  "VK_LAYER_LUNARG_standard_validation"
};
// NDEBUG C++ macro that stands for "no debug".
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

// An extension function has an address (vkGetInstanceProcAddr).
VkResult CreateDebugReportCallbackEXT( VkInstance instance,
                                       const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                       const VkAllocationCallbacks* pAllocator,
                                       VkDebugReportCallbackEXT* pCallback )
{
  auto func = ( PFN_vkCreateDebugReportCallbackEXT )
              vkGetInstanceProcAddr( instance, "vkCreateDebugReportCallbackEXT" );
  return (func != nullptr) ?
         func( instance, pCreateInfo, pAllocator, pCallback )
         : VK_ERROR_EXTENSION_NOT_PRESENT;
}

// This function should be static or an outsider.
void DestroyDebugReportCallbackEXT( VkInstance instance,
                                    VkDebugReportCallbackEXT callback,
                                    const VkAllocationCallbacks* pAllocator )
{
  auto func = ( PFN_vkDestroyDebugReportCallbackEXT )
              vkGetInstanceProcAddr( instance, "vkDestroyDebugReportCallbackEXT" );
  if (func != nullptr)
  {
    func( instance, callback, pAllocator );
  }
}

/** Surface-related. **/

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector< VkSurfaceFormatKHR > formats;
  std::vector< VkPresentModeKHR > presentModes;
};

/** Device-related. **/

// Device extensions.
const std::vector< const char* > deviceExtensions =
{
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/** Queue-related. **/

struct QueueFamilyIndices
{
  int graphicsFamily = -1;
  int presentFamily = -1;

  bool isComplete( void )
  {
    return graphicsFamily >= 0 && presentFamily >= 0;
  }
};

class HelloTriangleApplication
{

  public:
    void run( void )
    {
      initWindow( );
      initVulkan( );
      mainLoop( );
      cleanup( );
    }

  private:
    // Auxiliar functions.
    bool checkValidationLayerSupport( void )
    {
      uint32_t layerCount;
      vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

      std::vector< VkLayerProperties > availableLayers( layerCount );
      vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data( ) );

      // Check if all of the layers in validationLayers exist in the availableLayers list.
      for( const char* layerName : validationLayers )
      {
        bool layerFound = false;

        for( const auto& layerProperties : availableLayers )
        {
          if( strcmp( layerName, layerProperties.layerName ) == 0 )
          {
            layerFound = true;
            break;
          }
        }

        if ( !layerFound )
        {
            return false;
        }
      }

      return true;
    }

    bool checkDeviceExtensionSupport( VkPhysicalDevice device )
    {
      uint32_t extensionCount;
      vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );

      std::vector< VkExtensionProperties > availableExtensions( extensionCount );
      vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions.data( ) );

      std::set< std::string > requiredExtensions( deviceExtensions.begin( ), deviceExtensions.end( ) );

      // Nice trick to know if all required extensions have been found.
      for( const auto& extension : availableExtensions )
      {
        requiredExtensions.erase( extension.extensionName );
      }

      return requiredExtensions.empty( );
    }

    SwapChainSupportDetails querySwapChainSupport( VkPhysicalDevice device )
    {
      SwapChainSupportDetails details;

      // SwapChainSupportDetails.capabilities
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, surface, &details.capabilities );

      // SwapChainSupportDetails.formats
      uint32_t formatCount;
      vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatCount, nullptr );

      if( formatCount != 0 )
      {
        details.formats.resize( formatCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatCount, details.formats.data( ) );
      }

      // SwapChainSupportDetails.presentModes
      uint32_t presentModeCount;
      vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &presentModeCount, nullptr );

      if( presentModeCount != 0 )
      {
        details.presentModes.resize( presentModeCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &presentModeCount, details.presentModes.data( ) );
      }

      return details;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& availableFormats )
    {
      // VkSurfaceFormat formed by format and colorSpace.
      if( availableFormats.size( ) == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED )
      {
        return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
      }

      // availableFormats.size( ) > 1.
      for( const auto& availableFormat : availableFormats )
      {
        if( availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
        {
          return availableFormat;
        }
      }

      /** Option 1. **/
      // Make a ranking and choose the best foramt.

      /** Option 2. **/
      return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode( const std::vector<VkPresentModeKHR> availablePresentModes )
    {
      // MAILBOX > IMMEDIATE > FIFO.
      VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

      for( const auto& availablePresentMode : availablePresentModes )
      {
        if( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR )
        {
          return availablePresentMode;
        }
        else if( availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR )
        {
          bestMode = availablePresentMode;
        }
      }

      return bestMode;
    }

    VkExtent2D chooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities )
    {
      if( capabilities.currentExtent.width != std::numeric_limits< uint32_t >::max( ) )
      {
        return capabilities.currentExtent;
      }
      else
      {
        VkExtent2D actualExtent = {WIDTH, HEIGHT};

        actualExtent.width = std::max( capabilities.minImageExtent.width,
                                       std::min( capabilities.maxImageExtent.width, actualExtent.width ) );
        actualExtent.height = std::max( capabilities.minImageExtent.height,
                                        std::min( capabilities.maxImageExtent.height, actualExtent.height ) );

        return actualExtent;
      }
    }

    void createInstance( void )
    {
      if( enableValidationLayers && !checkValidationLayerSupport( ) )
      {
        throw std::runtime_error( "validation layers requested, but not available!" );
      }

      // VkApplicationInfo struct. Optional.
      VkApplicationInfo appInfo = {};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "Hello Triangle";
      appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.pEngineName = "No Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.apiVersion = VK_API_VERSION_1_0;

      // VkInstanceCreateInfo struct. Required.
      VkInstanceCreateInfo createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pApplicationInfo = &appInfo;

      // Extensions.
      auto extensions = getRequiredExtensions( );
      createInfo.enabledExtensionCount = static_cast< uint32_t >( extensions.size( ) );
      createInfo.ppEnabledExtensionNames = extensions.data( );

      // Validation layers.
      if (enableValidationLayers)
      {
        createInfo.enabledLayerCount = static_cast< uint32_t >( validationLayers.size( ) );
        createInfo.ppEnabledLayerNames = validationLayers.data( );
      }
      else
      {
        createInfo.enabledLayerCount = 0;
      }

      // Parameters: creation info, custom allocator callbacks, pointer to object.
      if( vkCreateInstance( &createInfo, nullptr, &instance ) != VK_SUCCESS )
      {
        throw std::runtime_error( "Failed to create VkInstance." );
      }

      // Available extensions.
      uint32_t extensionCount = 0;
      vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );

      std::vector< VkExtensionProperties > extensionProperties( extensionCount );
      vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, extensionProperties.data( ) );

      // Printing them.
      std::cout << "Available extensions:" << std::endl;

      for (const auto& extension : extensionProperties)
      {
        std::cout << "\t" << extension.extensionName << std::endl;
      }
    }

    std::vector< const char* > getRequiredExtensions( void )
    {
      std::vector< const char* > extensions;

      unsigned int glfwExtensionCount = 0;
      const char** glfwExtensions;
      glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

      for( unsigned int i = 0; i < glfwExtensionCount; i++ )
      {
        extensions.push_back( glfwExtensions[i] );
      }

      // Additional extension: VK_EXT_debug_report.
      if( enableValidationLayers )
      {
        extensions.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
      }

      return extensions;
    }

    void setupDebugCallback( void )
    {
      if( !enableValidationLayers ) return;

      VkDebugReportCallbackCreateInfoEXT createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
      createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
      createInfo.pfnCallback = debugCallback;

      if( CreateDebugReportCallbackEXT( instance, &createInfo, nullptr, &callback ) != VK_SUCCESS )
      {
        throw std::runtime_error("failed to set up debug callback!");
      }
    }

    void createSurface( void )
    {
      // That is how a surface for Windows OS would be created behind the scenes.
      /**
      VkWin32SurfaceCreateInfoKHR createInfo;
      createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
      createInfo.hwnd = glfwGetWin32Window( window );
      createInfo.hinstance = GetModuleHandle( nullptr );

      auto CreateWin32SurfaceKHR =
        (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr( instance, "vkCreateWin32SurfaceKHR" );

      if( !CreateWin32SurfaceKHR ||
          CreateWin32SurfaceKHR( instance, &createInfo, nullptr, &surface ) != VK_SUCCESS )
      {
        throw std::runtime_error( "failed to create window surface!" );
      }
      **/

      // Easy and cross-platform way.
      if( glfwCreateWindowSurface( instance, window, nullptr, &surface ) != VK_SUCCESS )
      {
        throw std::runtime_error("failed to create window surface!");
      }

    }

    void pickPhysicalDevice( void )
    {
      uint32_t deviceCount = 0;
      vkEnumeratePhysicalDevices( instance, &deviceCount, nullptr );

      if( deviceCount == 0 )
      {
        throw std::runtime_error( "failed to find GPUs with Vulkan support!" );
      }

      std::vector< VkPhysicalDevice > devices( deviceCount );
      vkEnumeratePhysicalDevices( instance, &deviceCount, devices.data( ) );

      // Checking if all GPUs are suitable for specific operations.
      // Get the first that passes the test.
      for( const auto& device : devices )
      {
        if( isDeviceSuitable( device ) )
        {
          physicalDevice = device;
          break;
        }
      }

      if( physicalDevice == VK_NULL_HANDLE )
      {
        throw std::runtime_error( "failed to find a suitable GPU!" );
      }
    }

    bool isDeviceSuitable( VkPhysicalDevice device )
    {
      /** Option 1. **/
      /**
      // Properties (basic).
      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties( device, &deviceProperties );

      // Features (optional).
      VkPhysicalDeviceFeatures deviceFeatures;
      vkGetPhysicalDeviceFeatures( device, &deviceFeatures );

      // Example: we require the use of geometry shaders.
      return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
             deviceFeatures.geometryShader;
      **/

      /** Option 2. **/
      // Give each device a score and chose the highest one.

      /** Option 3. **/
      // User choice.

      /** Option 4. **/
      // Any GPU.
      // return true;

      /** Option 5. **/
      // Conditions.

      // 1: Queues with VK_QUEUE_GRAPHICS_BIT and SurfaceSupportKHR.
      QueueFamilyIndices indices = findQueueFamilies( device );

      // 2: Device with VK_KHR_swapchain extension.
      bool extensionsSupported = checkDeviceExtensionSupport( device );

      // 3: SwapChainSupportDetails are sufficiently good.
      // At least one format and one present mode.
      bool swapChainAdequate = false;
      if( extensionsSupported )
      {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport( device );
        swapChainAdequate = !swapChainSupport.formats.empty( ) && !swapChainSupport.presentModes.empty( );
      }

      return indices.isComplete( ) && extensionsSupported && swapChainAdequate;
    }

    QueueFamilyIndices findQueueFamilies( VkPhysicalDevice device )
    {
      QueueFamilyIndices indices;

      uint32_t queueFamilyCount = 0;
      vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );

      std::vector< VkQueueFamilyProperties > queueFamilies( queueFamilyCount );
      vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.data( ) );

      int i = 0;
      for( const auto& queueFamily : queueFamilies )
      {
        /** graphicsFamily checking. **/
        if( queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
          indices.graphicsFamily = i;
        }

        /** presentFamily checking. **/
        // Surface support KHR.
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR( device, i, surface, &presentSupport );

        if( queueFamily.queueCount > 0 && presentSupport )
        {
          indices.presentFamily = i;
        }

        if( indices.isComplete( ) )
        {
          break;
        }

        i++;
      }

      return indices;
    }

    void createLogicalDevice( void )
    {
      QueueFamilyIndices indices = findQueueFamilies( physicalDevice );

      // Multiple VkDeviceQueueCreateInfo.
      std::vector< VkDeviceQueueCreateInfo > queueCreateInfos;
      std::set< int > uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

      float queuePriority = 1.0f;
      for( int queueFamily : uniqueQueueFamilies )
      {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back( queueCreateInfo );
      }

      // VkPhysicalDeviceFeatures. What features are going to be used.
      VkPhysicalDeviceFeatures deviceFeatures = {};

      // Actually creating the logical device.
      VkDeviceCreateInfo createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

      createInfo.queueCreateInfoCount = static_cast< uint32_t >( queueCreateInfos.size( ) );
      createInfo.pQueueCreateInfos = queueCreateInfos.data( );

      createInfo.pEnabledFeatures = &deviceFeatures;

      // Device extension: VK_KHR_swapchain.
      createInfo.enabledExtensionCount = static_cast< uint32_t >( deviceExtensions.size( ) );
      createInfo.ppEnabledExtensionNames = deviceExtensions.data( );

      if( enableValidationLayers )
      {
        createInfo.enabledLayerCount = static_cast< uint32_t >( validationLayers.size( ) );
        createInfo.ppEnabledLayerNames = validationLayers.data( );
      }
      else
      {
        createInfo.enabledLayerCount = 0;
      }

      if( vkCreateDevice( physicalDevice, &createInfo, nullptr, &device ) != VK_SUCCESS )
      {
        throw std::runtime_error( "failed to create logical device!" );
      }

      // Getting VkQueue(s) from the logical device.
      vkGetDeviceQueue( device, indices.graphicsFamily, 0, &graphicsQueue );
      vkGetDeviceQueue( device, indices.presentFamily, 0, &presentQueue );
    }

    void createSwapChain( void )
    {
      SwapChainSupportDetails swapChainSupport = querySwapChainSupport( physicalDevice );

      VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat( swapChainSupport.formats );
      VkPresentModeKHR presentMode = chooseSwapPresentMode( swapChainSupport.presentModes );
      VkExtent2D extent = chooseSwapExtent( swapChainSupport.capabilities );

      // Trying (+1) to implement triple buffering.
      uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
      // maxImageCount = 0 means no limits.
      if( swapChainSupport.capabilities.maxImageCount > 0 &&
          imageCount > swapChainSupport.capabilities.maxImageCount )
      {
        imageCount = swapChainSupport.capabilities.maxImageCount;
      }

      VkSwapchainCreateInfoKHR createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      createInfo.surface = surface;
      createInfo.minImageCount = imageCount;
      createInfo.imageFormat = surfaceFormat.format;
      createInfo.imageColorSpace = surfaceFormat.colorSpace;
      createInfo.imageExtent = extent;
      // 1 unless stereoscopic 3D app.
      createInfo.imageArrayLayers = 1;
      // Render directly.
      createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

      // Queue issues.
      QueueFamilyIndices indices = findQueueFamilies( physicalDevice );
      uint32_t queueFamilyIndices[] =
        { (uint32_t) indices.graphicsFamily, (uint32_t) indices.presentFamily };

      if( indices.graphicsFamily != indices.presentFamily )
      {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
      }
      else
      {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
      }

      // You can apply a transformation to all images.
      createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

      // Blending with other windows. Default: no (opaque).
      createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

      createInfo.presentMode = presentMode;
      createInfo.clipped = VK_TRUE;

      // Safety pointer to current swap chain.
      createInfo.oldSwapchain = VK_NULL_HANDLE;

      if( vkCreateSwapchainKHR( device, &createInfo, nullptr, &swapChain ) != VK_SUCCESS )
      {
        throw std::runtime_error( "failed to create swap chain!" );
      }

      // Getting VkImage(s).
      vkGetSwapchainImagesKHR( device, swapChain, &imageCount, nullptr );
      swapChainImages.resize( imageCount );
      vkGetSwapchainImagesKHR( device, swapChain, &imageCount, swapChainImages.data( ) );

      // Getting VkFormat and VkExtent2D.
      swapChainImageFormat = surfaceFormat.format;
      swapChainExtent = extent;
    }

    void createImageViews( void )
    {
      swapChainImageViews.resize( swapChainImages.size( ) );

      for( size_t i = 0; i < swapChainImages.size( ); i++ )
      {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        // Here you can do some permutations.
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        // VkImageView treatment, mipmaps or layers.
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if( vkCreateImageView( device, &createInfo, nullptr, &swapChainImageViews[i] ) != VK_SUCCESS )
        {
          throw std::runtime_error( "failed to create image views!" );
        }
      }
    }

    void createGraphicsPipeline( void )
    {
      // Getting shaders byte array.
      auto vertShaderCode = readFile( triangleVS );
      auto fragShaderCode = readFile( triangleFS );

      // Here we should check that byte size is equal to file byte size.

      // VkShaderModule(s) are going to be used only here.
      VkShaderModule vertShaderModule;
      VkShaderModule fragShaderModule;

      vertShaderModule = createShaderModule( vertShaderCode );
      fragShaderModule = createShaderModule( fragShaderCode );

      // Actually creating the vertex shader here.
      VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
      vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
      vertShaderStageInfo.module = vertShaderModule;
      vertShaderStageInfo.pName = "main";

      // Actually creating the fragment shader here.
      VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
      fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      fragShaderStageInfo.module = fragShaderModule;
      fragShaderStageInfo.pName = "main";

      VkPipelineShaderStageCreateInfo shaderStages[] =
        {vertShaderStageInfo, fragShaderStageInfo};

      // Vertex input. Nothing because it is hard coded in the VS.
      VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
      vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      vertexInputInfo.vertexBindingDescriptionCount = 0;
      vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
      vertexInputInfo.vertexAttributeDescriptionCount = 0;
      vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

      // Input assembly.
      VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
      inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      inputAssembly.primitiveRestartEnable = VK_FALSE;

      // Viewport.
      VkViewport viewport = {};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = ( float ) swapChainExtent.width;
      viewport.height = ( float ) swapChainExtent.height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      // Scissors.
      VkRect2D scissor = {};
      scissor.offset = {0, 0};
      scissor.extent = swapChainExtent;

      // Viewport state = Viewport + Scissors.
      VkPipelineViewportStateCreateInfo viewportState = {};
      viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportState.viewportCount = 1;
      viewportState.pViewports = &viewport;
      viewportState.scissorCount = 1;
      viewportState.pScissors = &scissor;

      /** Rasterizer. **/
      VkPipelineRasterizationStateCreateInfo rasterizer = {};
      rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      // Frags out of frustum are only clamped (not discarded).
      rasterizer.depthClampEnable = VK_FALSE;
      // If true: no rasterizer action, no ouput.
      rasterizer.rasterizerDiscardEnable = VK_FALSE;
      // If fill is not used -> you must use a GPU feature.
      rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
      // If it is greater than 1 -> you must use a GPU feature.
      rasterizer.lineWidth = 1.0f;
      rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
      rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
      // Some bias can be added to the depth values.
      rasterizer.depthBiasEnable = VK_FALSE;
      rasterizer.depthBiasConstantFactor = 0.0f; // Optional
      rasterizer.depthBiasClamp = 0.0f; // Optional
      rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

      /** Multisampling. **/
      // If you use it -> you must use a GPU feature.
      VkPipelineMultisampleStateCreateInfo multisampling = {};
      multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      multisampling.sampleShadingEnable = VK_FALSE;
      multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      multisampling.minSampleShading = 1.0f; // Optional
      multisampling.pSampleMask = nullptr; // Optional
      multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
      multisampling.alphaToOneEnable = VK_FALSE; // Optional

      // Depth and stencil testing. Not needed now.
      // VkPipelineDepthStencilStateCreateInfo depthStencil = nullptr;

      /** Color blending. **/
      // VkPipelineColorBlendAttachmentState for each framebuffer.
      VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
      colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                            VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT |
                                            VK_COLOR_COMPONENT_A_BIT;
      colorBlendAttachment.blendEnable = VK_FALSE;
      colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
      colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
      colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
      colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
      colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
      colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

      // VkPipelineColorBlendStateCreateInfo for global config.
      VkPipelineColorBlendStateCreateInfo colorBlending = {};
      colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      colorBlending.logicOpEnable = VK_FALSE;
      colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
      colorBlending.attachmentCount = 1;
      colorBlending.pAttachments = &colorBlendAttachment;
      colorBlending.blendConstants[0] = 0.0f; // Optional
      colorBlending.blendConstants[1] = 0.0f; // Optional
      colorBlending.blendConstants[2] = 0.0f; // Optional
      colorBlending.blendConstants[3] = 0.0f; // Optional

      /** Dynamic state. **/
      // Option 1. This is to change some things in drawing time.
      /**
      VkDynamicState dynamicStates[] = {
          VK_DYNAMIC_STATE_VIEWPORT,
          VK_DYNAMIC_STATE_LINE_WIDTH
      };

      VkPipelineDynamicStateCreateInfo dynamicState = {};
      dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicState.dynamicStateCount = 2;
      dynamicState.pDynamicStates = dynamicStates;
      **/
      // Option 2.
      // VkPipelineDynamicStateCreateInfo dynamicState = nullptr;

      /** Pipeline layout. **/
      VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
      pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutInfo.setLayoutCount = 0; // Optional
      pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
      pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
      pipelineLayoutInfo.pPushConstantRanges = 0; // Optional

      if( vkCreatePipelineLayout( device, &pipelineLayoutInfo, nullptr, &pipelineLayout ) != VK_SUCCESS )
      {
        throw std::runtime_error( "failed to create pipeline layout!" );
      }

      VkGraphicsPipelineCreateInfo pipelineInfo = {};
      pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineInfo.stageCount = 2;
      pipelineInfo.pStages = shaderStages;
      pipelineInfo.pVertexInputState = &vertexInputInfo;
      pipelineInfo.pInputAssemblyState = &inputAssembly;
      pipelineInfo.pViewportState = &viewportState;
      pipelineInfo.pRasterizationState = &rasterizer;
      pipelineInfo.pMultisampleState = &multisampling;
      pipelineInfo.pDepthStencilState = nullptr; // Optional
      pipelineInfo.pColorBlendState = &colorBlending;
      pipelineInfo.pDynamicState = nullptr; // Optional
      pipelineInfo.layout = pipelineLayout;
      pipelineInfo.renderPass = renderPass;
      pipelineInfo.subpass = 0;
      pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
      pipelineInfo.basePipelineIndex = -1; // Optional

      if( vkCreateGraphicsPipelines( device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline ) != VK_SUCCESS )
      {
        throw std::runtime_error( "failed to create graphics pipeline!" );
      }

      // Destroying VkShaderModule(s).
      vkDestroyShaderModule( device, fragShaderModule, nullptr );
      vkDestroyShaderModule( device, vertShaderModule, nullptr );
    }

    void createRenderPass( void )
    {
      VkAttachmentDescription colorAttachment = {};
      colorAttachment.format = swapChainImageFormat;
      colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
      colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

      VkAttachmentReference colorAttachmentRef = {};
      colorAttachmentRef.attachment = 0;
      colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      // A pass can have multiple subpasses.
      VkSubpassDescription subpass = {};
      subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpass.colorAttachmentCount = 1;
      subpass.pColorAttachments = &colorAttachmentRef;

      VkRenderPassCreateInfo renderPassInfo = {};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
      renderPassInfo.attachmentCount = 1;
      renderPassInfo.pAttachments = &colorAttachment;
      renderPassInfo.subpassCount = 1;
      renderPassInfo.pSubpasses = &subpass;

      if( vkCreateRenderPass( device, &renderPassInfo, nullptr, &renderPass ) != VK_SUCCESS )
      {
        throw std::runtime_error( "failed to create render pass!" );
      }
    }

    VkShaderModule createShaderModule( const std::vector< char >& code )
    {
      VkShaderModuleCreateInfo createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      createInfo.codeSize = code.size( );
      createInfo.pCode = reinterpret_cast< const uint32_t* >( code.data( ) );

      VkShaderModule shaderModule;
      if( vkCreateShaderModule( device, &createInfo, nullptr, &shaderModule ) != VK_SUCCESS )
      {
        throw std::runtime_error( "failed to create shader module!" );
      }

      return shaderModule;
    }

    // Static funcions.

    // Following the PFN_vkDebugReportCallbackEXT prototype.
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugReportFlagsEXT flags,
      VkDebugReportObjectTypeEXT objType,
      uint64_t obj,
      size_t location,
      int32_t code,
      const char* layerPrefix,
      const char* msg,
      void* userData)
    {
      std::cerr << "validation layer: " << msg << std::endl;

      return VK_FALSE;
    }

    static std::vector< char > readFile( const std::string& filename )
    {
      std::ifstream file( filename, std::ios::ate | std::ios::binary );

      if( !file.is_open( ) )
      {
        throw std::runtime_error( "failed to open file!" );
      }

      // Getting file size.
      size_t fileSize = (size_t) file.tellg( );
      std::vector< char > buffer( fileSize );

      // Going back to the start and read.
      file.seekg( 0 );
      file.read( buffer.data( ), fileSize );

      // Closing and returning the bytes.
      file.close( );

      return buffer;
    }

    // Run functions.
    void initWindow( void )
    {
      glfwInit( );

      // Not creating an OGL context.
      glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
      // Resizing is special in Vulkan.
      glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

      // 4th: specific monitor. 5th: OGL relevant.
      window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr );
    }

    void initVulkan( void )
    {
      createInstance( );
      setupDebugCallback( );
      createSurface( );
      pickPhysicalDevice( );
      createLogicalDevice( );
      createSwapChain( );
      createImageViews( );
      createRenderPass( );
      createGraphicsPipeline( );
    }

    void mainLoop( void )
    {
      while( !glfwWindowShouldClose( window ) )
      {
        glfwPollEvents( );
      }
    }

    void cleanup( void )
    {
      vkDestroyPipeline( device, graphicsPipeline, nullptr );
      vkDestroyPipelineLayout( device, pipelineLayout, nullptr );
      vkDestroyRenderPass( device, renderPass, nullptr );

      for( size_t i = 0; i < swapChainImageViews.size(); i++ )
      {
        vkDestroyImageView(device, swapChainImageViews[i], nullptr );
      }

      vkDestroySwapchainKHR( device, swapChain, nullptr );

      vkDestroyDevice( device, nullptr );

      vkDestroySurfaceKHR( instance, surface, nullptr );

      DestroyDebugReportCallbackEXT( instance, callback, nullptr );
      vkDestroyInstance( instance, nullptr );

      glfwDestroyWindow( window );

      glfwTerminate( );
    }

    /** Attributes. **/

    // GLFW.
    GLFWwindow* window;

    // Instance-related.
    VkInstance instance;
    VkDebugReportCallbackEXT callback;

    // KHR-related.
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    std::vector< VkImage > swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    // Pipeline-related.
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    // Device-related.
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // It will be destroyed with VkInstance.
    VkDevice device;

    // Queue-related.
    VkQueue graphicsQueue;
    VkQueue presentQueue;
};

int main( )
{
  HelloTriangleApplication app;

  try
  {
    app.run();
  }
  catch (const std::runtime_error& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
