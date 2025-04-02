#include "RenderContext_vulkan.hpp"
#include <iostream>

#define RENDER_DEBUG

RenderContext_vulkan::RenderContext_vulkan()
	:RenderContext(RENDER_CONTEXT_TYPE_VULKAN)
{
}

RenderContext_vulkan::~RenderContext_vulkan()
{
}

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


void RenderContext_vulkan::StartUp( Window* window )
{
	CreateInstance();
	SetupDebugMessenger();
}

void RenderContext_vulkan::ShutDown()
{
#if defined(RENDER_DEBUG)
	DestroyDebugUtilsMessengerEXT( m_instance, m_debugMessenger, nullptr );
#endif

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
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

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


