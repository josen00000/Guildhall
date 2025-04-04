#include "RenderContext_vulkan.hpp"
#include <iostream>
#include <optional>
#include <set>
#include <algorithm>
#include <array>
#include <fstream>

#define RENDER_DEBUG
#ifdef RENDER_DEBUG
const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
};
#endif

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete() 
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
 };

RenderContext_vulkan::RenderContext_vulkan()
	:RenderContext(RENDER_CONTEXT_TYPE_VULKAN)
{
}

RenderContext_vulkan::~RenderContext_vulkan()
{
}

// Helper functions
static VkResult CreateDebugUtilsMessengerEXT( VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger )
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" );
	if( func != nullptr ) {
		return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

static void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator ) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );
	if( func != nullptr ) {
		func( instance, debugMessenger, pAllocator );
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData )
{
	if( messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT ){
		std::string errorMsg = "Validation layer";
		errorMsg += pCallbackData->pMessage;
		ERROR_RECOVERABLE( errorMsg );
	}

	return VK_FALSE;
}

static QueueFamilyIndices FindQueueFamilies( VkPhysicalDevice device, VkSurfaceKHR surface )
{
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );
	std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.data() );

	int i = 0;
	for( const VkQueueFamilyProperties& queueFamily : queueFamilies )
	{
		if( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR( device, i, surface, &presentSupport );
		if( presentSupport )
		{
			indices.presentFamily = i;
		}

		if( indices.IsComplete() )
		{
			break;
		}
		i++;
	}
	return indices;
}

static bool CheckDeviceExtensionSupport( VkPhysicalDevice device )
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );
	std::vector<VkExtensionProperties> availableExtensions( extensionCount );
	vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions.data() );

	std::set<std::string> requiredExtensions( deviceExtensions.begin(), deviceExtensions.end() );

	for( const VkExtensionProperties& extension : availableExtensions )
	{
		requiredExtensions.erase( extension.extensionName );
	}

	return requiredExtensions.empty();
}

static SwapChainSupportDetails QuerySwapChainSupport( VkPhysicalDevice device, VkSurfaceKHR surface )
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, surface, &details.capabilities );

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatCount, nullptr );
	if( formatCount != 0 )
	{
		details.formats.resize( formatCount );
		vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatCount, details.formats.data() );
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &presentModeCount, nullptr );
	if( presentModeCount != 0 )
	{
		details.presentModes.resize( presentModeCount );
		vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &presentModeCount, details.presentModes.data() );
	}

	return details;
}

static bool IsDeviceSuitable( const VkPhysicalDevice& device, VkSurfaceKHR surface )
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties( device, &deviceProperties );

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures( device, &deviceFeatures );

	bool passDeviceCheck = ( deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ) && deviceFeatures.geometryShader;
	bool passExtensionsCheck = CheckDeviceExtensionSupport(device); 
	bool passSwapChainSupportCheck = false;
	if( passExtensionsCheck ){
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport( device, surface );
		passSwapChainSupportCheck = ( !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty() );
	}
	QueueFamilyIndices indices = FindQueueFamilies( device, surface );

	return passDeviceCheck && passExtensionsCheck && indices.IsComplete() && passSwapChainSupportCheck;
}

static VkSurfaceFormatKHR ChooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& availableFormats )
{
	for( const VkSurfaceFormatKHR& availableFormat : availableFormats )
	{
		// TODO: check if the format is supported and double check what format is my engine supported
		if( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
		{
			return availableFormat;
		}
	}
	return availableFormats[0];
}

static VkPresentModeKHR ChooseSwapPresentMode( const std::vector<VkPresentModeKHR>& availablePresentModes )
{
	for( const VkPresentModeKHR& availablePresentMode : availablePresentModes )
	{
		if( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR )
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities, Window* window )
{
	if( capabilities.currentExtent.width != UINT32_MAX )
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width = window->GetClientWidth();
		int height = window->GetClientHeight();
		VkExtent2D actualExtent = { static_cast<uint32_t>( width ), static_cast<uint32_t>( height ) };
		actualExtent.width = std::clamp( actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
		actualExtent.height = std::clamp( actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );
		return actualExtent;
	}
}

static VkVertexInputBindingDescription GetBindingDescription()
{
	// TODO: Need to implement vertex_PCUTBN later.
	// TODO: we will use vertex input rate vertex for now. Later we will implement instance buffer
	// TODO: Move to static function of Vertex_PCU
	//VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
	//VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof( Vertex_PCU );
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
{
	// TODO: Move to static function of Vertex_PCU
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
	// position
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof( Vertex_PCU, m_pos );

	// color
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R8G8B8A8_UINT;
	attributeDescriptions[1].offset = offsetof( Vertex_PCU, m_color );

	return attributeDescriptions;
}

static std::vector<char> ReadFile( const std::string& fileName )
{
	std::ifstream file( fileName, std::ios::ate | std::ios::binary );
	if( !file.is_open() )
	{
		ERROR_AND_DIE( "Failed to open file!" );
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer( fileSize );

	file.seekg( 0 );
	file.read( buffer.data(), fileSize );
	file.close();

	return buffer;
}

static VkShaderModule  CreateShaderModule( const std::vector<char>& code, VkDevice device )
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>( code.data() );

	VkShaderModule shaderModule;
	if( vkCreateShaderModule( device, &createInfo, nullptr, &shaderModule ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create shader module!" );
	}
	return shaderModule;

}
// End of helper functions


void RenderContext_vulkan::StartUp( Window* window )
{
	CreateInstance();
	SetupDebugMessenger();
	CreateSurface( window );
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain( window );
	CreateGraphicsPipeline();
}

void RenderContext_vulkan::ShutDown()
{
#if defined(RENDER_DEBUG)
	DestroyDebugUtilsMessengerEXT( m_instance, m_debugMessenger, nullptr );
#endif
	// destroy device first and then instance
	for( auto imageView : m_swapChainImageViews )
	{
		vkDestroyImageView( m_device, imageView, nullptr );
	}
	vkDestroySwapchainKHR( m_device, m_VkSwapChain, nullptr );

	vkDestroyShaderModule( m_device, m_vertShaderModule, nullptr );
	vkDestroyShaderModule( m_device, m_fragShaderModule, nullptr );
	vkDestroyDevice( m_device, nullptr );
	vkDestroySurfaceKHR( m_instance, m_surface, nullptr );
	vkDestroyInstance( m_instance, nullptr );
}

void RenderContext_vulkan::BeginFrame()
{
}

void RenderContext_vulkan::EndFrame()
{
}

void RenderContext_vulkan::BeginCamera( Camera* camera, Convention convention )
{
}

void RenderContext_vulkan::EndCamera()
{
}

void RenderContext_vulkan::ClearState()
{
}

void RenderContext_vulkan::EnableDepth( DepthCompareFunc func, bool writeDepthOnPass )
{
}

void RenderContext_vulkan::DisableDepth()
{
}

void RenderContext_vulkan::SetDiffuseTexture( Texture* texture, int index )
{
}

void RenderContext_vulkan::SetTintColor( const Rgba8& tint )
{
}

void RenderContext_vulkan::SetRasterCullMode( RasterCullMode mode )
{
}

void RenderContext_vulkan::SetRasterFillMode( RasterFillMode mode )
{
}

void RenderContext_vulkan::SetFrontFaceWindOrder( RasterWindOrder order )
{
}

void RenderContext_vulkan::Draw( int numVertexes, int vertexOffset )
{
}

void RenderContext_vulkan::DrawMesh( GPUMesh* mesh )
{
}

void RenderContext_vulkan::DrawIndexed( int indexCount, int indexOffset, int vertexOffset )
{
}

void RenderContext_vulkan::DrawIndexedVertexVector( const std::vector<Vertex_PCU>& vertices, const std::vector<uint>& indexes )
{
}

void RenderContext_vulkan::DrawVertexVector( const std::vector<Vertex_PCU>& vertices )
{
}

void RenderContext_vulkan::DrawVertexArray( int vertexNum, Vertex_PCU* vertexArray )
{
}

void RenderContext_vulkan::DrawLine( const Vec2& startPoint, const Vec2& endPoint, const float thick, const Rgba8& lineColor )
{
}

void RenderContext_vulkan::DrawAABB2D( const AABB2& bounds, const Rgba8& tint, const Vec2& uvMin, const Vec2& mvMax )
{
}

void RenderContext_vulkan::DrawCircle( Vec3 center, float radius, float thick, const Rgba8& circleColor )
{
}

bool RenderContext_vulkan::CheckValidationLayersSupport( const std::vector<const char*>& validationLayers )
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties( &layerCount, nullptr );
	std::vector<VkLayerProperties> availableLayers( layerCount );
	vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

	for(const char* layerName : validationLayers )
	{
		bool layerFound = false;
		for(const VkLayerProperties& layerProperties : availableLayers )
		{
			if( strcmp( layerName, layerProperties.layerName ) == 0 )
			{
				layerFound = true;
				break;
			}
		}
		if( !layerFound )
		{
			return false;
		}
	}
	return true;
}

void RenderContext_vulkan::GetAllExtensions()
{

	// debug code for get all available extensions
	//uint32_t extensionCount = 0;
	//vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );
	//std::vector<VkExtensionProperties> extensions( extensionCount );
	//vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, extensions.data() );
}

void RenderContext_vulkan::CreateInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan App";
	appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	const char* instanceExtensions[] = {
	VK_KHR_SURFACE_EXTENSION_NAME,
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#if defined(RENDER_DEBUG)
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME // Optional, for debugging
#endif
	};
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>( sizeof( instanceExtensions ) / sizeof( instanceExtensions[0] ) );
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions;

	// debug layer
#if defined(RENDER_DEBUG)
	if( !CheckValidationLayersSupport( validationLayers ) )
	{
		ERROR_AND_DIE( "Validation layers requested, but not available!" );
	}

	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>( validationLayers.size() );
	instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#else
	instanceCreateInfo.enabledLayerCount = 0;
#endif

	VkResult result = vkCreateInstance( &instanceCreateInfo, nullptr, &m_instance );
	if( result != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create Vulkan instance" );
	}
}

void RenderContext_vulkan::SetupDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;

	if(CreateDebugUtilsMessengerEXT( m_instance, &createInfo, nullptr, &m_debugMessenger ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to set up debug messenger!" );
	}

}

void RenderContext_vulkan::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices( m_instance, &deviceCount, nullptr );
	if( deviceCount == 0 )
	{
		ERROR_AND_DIE( "Failed to find GPUs with Vulkan support!" );
	}

	std::vector<VkPhysicalDevice> devices( deviceCount );
	vkEnumeratePhysicalDevices( m_instance, &deviceCount, devices.data() );

	for( const VkPhysicalDevice& device : devices )
	{
		if( IsDeviceSuitable( device, m_surface ) ){
			m_physicalDevice = device;
			// we only have one GPU for now so don't bother to check and rate the GPU
			break;
		}
	}

	if( m_physicalDevice == VK_NULL_HANDLE )
	{
		ERROR_AND_DIE( "Failed to find a suitable GPU!" );
	}
}

void RenderContext_vulkan::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies( m_physicalDevice, m_surface );

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	for( uint32_t queueFamily : uniqueQueueFamilies )
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back( queueCreateInfo );
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>( queueCreateInfos.size() );
	createInfo.pEnabledFeatures = &deviceFeatures;
#ifdef RENDER_DEBUG
	createInfo.enabledLayerCount = static_cast<uint32_t>( validationLayers.size() );
	createInfo.ppEnabledLayerNames = validationLayers.data();
#else
	createInfo.enabledLayerCount = 0;
#endif
	createInfo.enabledExtensionCount = static_cast<uint32_t>( deviceExtensions.size() );
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if(vkCreateDevice( m_physicalDevice, &createInfo, nullptr, &m_device ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create logical device!" );
	}

	vkGetDeviceQueue( m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue );
	vkGetDeviceQueue( m_device, indices.presentFamily.value(), 0, &m_presentQueue );
}

void RenderContext_vulkan::CreateSurface( Window* window )
{
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hwnd = (HWND)window->GetHandle();
	surfaceCreateInfo.hinstance = GetModuleHandle( nullptr );
	if( vkCreateWin32SurfaceKHR( m_instance, &surfaceCreateInfo, nullptr, &m_surface ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create window surface!" );
	}
}

void RenderContext_vulkan::CreateSwapChain(Window* window)
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport( m_physicalDevice, m_surface );
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat( swapChainSupport.formats );
	VkPresentModeKHR presentMode = ChooseSwapPresentMode( swapChainSupport.presentModes );
	VkExtent2D extent = ChooseSwapExtent( swapChainSupport.capabilities, window);

	//we may sometimes have to wait on the driver to complete internal operations before we can acquire another image to render to. 
	//Therefore it is recommended to request at least one more image than the minimum:
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if( swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount )
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	// always 1 unlsee developing a stereoscopic 3D application
	createInfo.imageArrayLayers = 1;
	// now render directly and will change later
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies( m_physicalDevice, m_surface );
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
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

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if(vkCreateSwapchainKHR( m_device, &createInfo, nullptr, &m_VkSwapChain ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create swap chain!" );
	}

	// get the swap chain images
	vkGetSwapchainImagesKHR( m_device, m_VkSwapChain, &imageCount, nullptr );
	m_swapChainImages.resize( imageCount );
	vkGetSwapchainImagesKHR( m_device, m_VkSwapChain, &imageCount, m_swapChainImages.data() );
	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;
}

void RenderContext_vulkan::CreateImageViews()
{
	m_swapChainImageViews.resize( m_swapChainImages.size() );
	for(int i = 0; i < m_swapChainImages.size(); i++ )
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if( vkCreateImageView( m_device, &createInfo, nullptr, &m_swapChainImageViews[i] ) != VK_SUCCESS )
		{
			ERROR_AND_DIE( "Failed to create image views!" );
		}
	}
}

void RenderContext_vulkan::CreateGraphicsPipeline()
{
	// vertex shader
	auto vertShaderCode = ReadFile( "data/Shader/vert.spv" );
	auto fragShaderCode = ReadFile( "data/Shader/frag.spv" );
	
	m_vertShaderModule = CreateShaderModule( vertShaderCode, m_device );
	m_fragShaderModule = CreateShaderModule( fragShaderCode, m_device );

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = m_vertShaderModule;
	vertShaderStageInfo.pName = "main";
	//vertShaderStageInfo.pSpecializationInfo = nullptr; // specify values for shader constants

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = m_fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo  shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	//auto bindingDescription = GetBindingDescription();
	//auto attributeDescriptions = GetAttributeDescriptions();
	//VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	//vertexInputInfo.vertexBindingDescriptionCount = 1;
	//vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>( attributeDescriptions.size() );
	//vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	//vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

}




