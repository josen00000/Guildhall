#include "RenderContext_vulkan.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
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



// debug draw data
const std::vector<Vertex_PCU> debugDrawData = {
	Vertex_PCU( Vec3( -0.5f, -0.5f, 0.f ), Rgba8::GREEN, Vec2( 0.f, 0.f ) ),
	Vertex_PCU( Vec3( 0.5f, -0.5f, 0.f ), Rgba8::BLUE , Vec2( 1.f, 1.f ) ),
	Vertex_PCU( Vec3( 0.5f, 0.5f, 0.f ), Rgba8::RED, Vec2( -1.f, -1.f ) ),
	Vertex_PCU( Vec3( -0.5f, 0.5f, 0.f ), Rgba8::BLACK, Vec2( -1.f, -1.f ) )
};

const std::vector<uint> debugDrawIndexes = {
	0, 1, 2,	
	2, 3, 0
};


RenderContext_vulkan::RenderContext_vulkan()
	:RenderContext(RENDER_CONTEXT_TYPE_VULKAN)
{
}

RenderContext_vulkan::~RenderContext_vulkan()
{
}
#pragma region HelperFunction
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
		std::string errorMsg = "Validation layer: ";
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

uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties( physicalDevice, &memProperties );
	for( uint32_t i = 0; i < memProperties.memoryTypeCount; i++ )
	{
		if( ( typeFilter & ( 1 << i ) ) && ( memProperties.memoryTypes[i].propertyFlags & properties ) == properties )
		{
			return i;
		}
	}
	ERROR_AND_DIE( "Failed to find suitable memory type!" );
	return 0;
}

static void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if( vkCreateBuffer( device, &bufferInfo, nullptr, &buffer ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create buffer!" );
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements( device, buffer, &memRequirements );

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType( memRequirements.memoryTypeBits, properties, physicalDevice );

	if( vkAllocateMemory( device, &allocInfo, nullptr, &bufferMemory ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to allocate buffer memory!" );
	}

	vkBindBufferMemory( device, buffer, bufferMemory, 0 );
}

static void CopyBuffer( VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size )
{
	//TODO: Need to implement another command pool for short-lived buffer.
	// use VK_CMMAND_POOL_TRANSIENT_BIT for short-lived command buffer
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo );

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer( commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion );

	vkEndCommandBuffer( commandBuffer );

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	// TODO: use fence to  schedule multiple transfer simultaneously and wait for all complete.
	vkQueueSubmit( graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE );
	vkQueueWaitIdle( graphicsQueue );

	vkFreeCommandBuffers( device, commandPool, 1, &commandBuffer );

}
// End of helper functions
#pragma endregion

void RenderContext_vulkan::StartUp( Window* window )
{
	m_window = window;
	CreateInstance();
	SetupDebugMessenger();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateUniformBuffers();
	CreateGraphicsPipeline();
	CreateFrameBuffers();
	CreateCommandPool();
	CreateCommandBuffers();
	createVertexBuffer();
	CreateIndexBuffer();
	CreateSyncObjects();
}

void RenderContext_vulkan::ShutDown()
{
#if defined(RENDER_DEBUG)
	DestroyDebugUtilsMessengerEXT( m_instance, m_debugMessenger, nullptr );
#endif
	// destroy device first and then instance
	ShutDownSwapChain();

	for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
	{
		vkDestroySemaphore( m_device, m_renderFinishedSemaphores[i], nullptr );
		vkDestroySemaphore( m_device, m_imageAvailableSemaphores[i], nullptr );
		vkDestroyFence( m_device, m_inFlightFences[i], nullptr );
	}
	m_immediateVBO->Cleanup();
	vkDestroyBuffer( m_device, m_lastBoundIBO, nullptr );

	for( int i = 0; i < m_uniformBuffers.size(); i++ ){
		m_uniformBuffers[i]->Cleanup();
	}
	vkDestroyCommandPool( m_device, m_commandPool, nullptr ); // also free the command buffer
	vkDestroyPipeline( m_device, m_graphicsPipeline, nullptr );
	vkDestroyDescriptorPool( m_device, m_descriptorPool, nullptr );
	vkDestroyDescriptorSetLayout( m_device, m_descriptorSetLayout, nullptr );
	vkDestroyPipelineLayout( m_device, m_pipelineLayout, nullptr );
	vkDestroyRenderPass( m_device, m_renderPass, nullptr );
	vkDestroyShaderModule( m_device, m_vertShaderModule, nullptr );
	vkDestroyShaderModule( m_device, m_fragShaderModule, nullptr );
	vkDestroyDevice( m_device, nullptr );
	vkDestroySurfaceKHR( m_instance, m_surface, nullptr );
	vkDestroyInstance( m_instance, nullptr );
}

void RenderContext_vulkan::BeginFrame()
{
	vkWaitForFences( m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
}

void RenderContext_vulkan::EndFrame()
{
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR( m_device, m_VkSwapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);
	if(result == VK_ERROR_OUT_OF_DATE_KHR ){
		RecreateSwapChain();
		return;
	}
	else if( result != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to acquire swap chain image!" );
	}

	UpdateUniformBuffer(m_currentFrame);
	vkResetFences( m_device, 1, &m_inFlightFences[m_currentFrame] );
	vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);
	RecordCommandBuffer( m_commandBuffers[m_currentFrame], imageIndex );

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	if( vkQueueSubmit( m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to submit draw command buffer!" );
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_VkSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional
	
	vkQueuePresentKHR( m_presentQueue, &presentInfo );

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RenderContext_vulkan::BeginCamera( Camera* camera, Convention convention )
{
	m_currentCamera = camera;
	// TODO: Need to implement camera render target

	RenderBuffer* cameraUBO = camera->GetOrCreateCameraBuffer( this, convention );

}

void RenderContext_vulkan::EndCamera()
{
}

void RenderContext_vulkan::ClearState()
{
}

void RenderContext_vulkan::CreateRenderBuffer( RenderBuffer& buffer )
{	
	VkBufferUsageFlags usage = buffer.GetVulkanUsage();
	VkMemoryPropertyFlags memUsage = buffer.GetMemoryUsage();
	VkBuffer bufferHandle; 
	VkDeviceMemory bufferMemory;
	CreateBuffer( m_device, m_physicalDevice, buffer.m_bufferByteSize, usage, memUsage, bufferHandle, bufferMemory);
	if( buffer.m_memHint == MEMORY_HINT_DYNAMIC )
	{
		// use persist mapped memory
		vkMapMemory( m_device, bufferMemory, 0, buffer.m_bufferByteSize, 0, &buffer.m_mappedMemoryData );
	}
	buffer.m_handle = (void*)bufferHandle;
	buffer.m_mappedMemory = (void*)bufferMemory;
}

void RenderContext_vulkan::UpdateRenderBuffer( RenderBuffer& buffer, void const* data, size_t dataByteSize, size_t elementByteSize )
{
	if( buffer.m_memHint == MEMORY_HINT_DYNAMIC ){
		// already mapped when created
		memcpy( buffer.m_mappedMemoryData, data, dataByteSize );
	}
	else if(buffer.m_memHint == MEMORY_HINT_GPU )
	{
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer( m_device, m_physicalDevice, buffer.m_bufferByteSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );
		void* bufferData;
		vkMapMemory( m_device, stagingBufferMemory, 0, buffer.m_bufferByteSize, 0, &bufferData);
		memcpy( bufferData, data, (size_t)buffer.m_bufferByteSize );
		vkUnmapMemory( m_device, stagingBufferMemory );

		CopyBuffer( m_device, m_commandPool, m_graphicsQueue, stagingBuffer,  (VkBuffer)buffer.m_handle, buffer.m_bufferByteSize );
		vkDestroyBuffer( m_device, stagingBuffer, nullptr );
		vkFreeMemory( m_device, stagingBufferMemory, nullptr );
	}
	else
	{
		ERROR_AND_DIE( "Unknown memory hint!" );
	}
}

void RenderContext_vulkan::CleanUpRenderBuffer( RenderBuffer& buffer )
{
	if( buffer.m_memHint == MEMORY_HINT_DYNAMIC )
	{
		if( buffer.m_mappedMemory != nullptr )
		{
			VkDeviceMemory mappedMemory = (VkDeviceMemory)buffer.m_mappedMemory;
			vkUnmapMemory( m_device, mappedMemory );
		}
	}

	vkDestroyBuffer( m_device, (VkBuffer)buffer.m_handle, nullptr );
	vkFreeMemory( m_device, (VkDeviceMemory)buffer.m_mappedMemory, nullptr );
	buffer.m_handle = nullptr;
	buffer.m_mappedMemory = nullptr;
	buffer.m_mappedMemoryData = nullptr;
}

void RenderContext_vulkan::BindVertexBuffer( VertexBuffer* buffer )
{
	VkBuffer vboHandle = (VkBuffer)buffer->m_handle;
	if( m_lastBoundVBO != vboHandle ){
		VkBuffer vertexBuffers[] = { (VkBuffer)m_immediateVBO->m_handle };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers( m_commandBuffers[m_currentFrame], 0, 1, vertexBuffers, offsets);
		m_lastBoundIBO = vboHandle;
	}
}

void RenderContext_vulkan::BindIndexBuffer( RenderBuffer* buffer )
{
	VkBuffer iboHandle = (VkBuffer)buffer->m_handle;
	if( m_lastBoundIBO != iboHandle )
	{
		vkCmdBindIndexBuffer( m_commandBuffers[m_currentFrame], iboHandle, 0, VK_INDEX_TYPE_UINT32 );
		m_lastBoundIBO = iboHandle;
	}
}

void RenderContext_vulkan::BindUniformBuffer( RenderBuffer* buffer, uint bindingPoint )
{
	VkBuffer uboHandle = (VkBuffer)buffer->m_handle;
	CreateDescriptorSets();
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

void RenderContext_vulkan::CreateSurface()
{
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hwnd = (HWND)m_window->GetHandle();
	surfaceCreateInfo.hinstance = GetModuleHandle( nullptr );
	if( vkCreateWin32SurfaceKHR( m_instance, &surfaceCreateInfo, nullptr, &m_surface ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create window surface!" );
	}
}

void RenderContext_vulkan::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport( m_physicalDevice, m_surface );
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat( swapChainSupport.formats );
	VkPresentModeKHR presentMode = ChooseSwapPresentMode( swapChainSupport.presentModes );
	VkExtent2D extent = ChooseSwapExtent( swapChainSupport.capabilities, m_window);

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

void RenderContext_vulkan::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = m_swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear the image before rendering
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // store the image after rendering
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // not used
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // not used
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // image is undefined before rendering
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // image is ready to be presented

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {};
	// dst is larger than src. 0 means first subpass
	// VK_SUBPASS_EXTERNAL implicit subpass before or after the render pass 
	// depending on whether it is specified in srcSubpass or dstSubpass
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0; // no access mask for src
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if( vkCreateRenderPass( m_device, &renderPassInfo, nullptr, &m_renderPass ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create render pass!" );
	}
}

void RenderContext_vulkan::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional For image sampling

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if( vkCreateDescriptorSetLayout( m_device, &layoutInfo, nullptr, &m_descriptorSetLayout ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create descriptor set layout!" );
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

	auto bindingDescription = GetBindingDescription();
	auto attributeDescriptions = GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_swapChainExtent.width;
	viewport.height = (float)m_swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChainExtent;

	// dynamic state for viewport
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>( dynamicStates.size() );
	dynamicState.pDynamicStates = dynamicStates.data();
	

	// static state for viewport
	
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE; // might be useful for shadow mapping
	rasterizer.rasterizerDiscardEnable = VK_FALSE; // discard the whole primitive
	// fill the whole triangle. Other options are line and point and need to require GPU feature
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL; 
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // cull the back face
	//TODO: Need to double check if I need to change the cullmode to counter clockwise
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; // clockwise or counter clockwise
	rasterizer.depthBiasEnable = VK_FALSE; // depth bias is used for shadow mapping
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE; // enable sample shading
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // 1 sample per pixel
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE; // enable blending
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE; // enable logic op
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional
	
	/*
	pseudocode for blending
	if (blendEnable) {
		finalColor.rgb = ( srcColorBlendFactor * newColor.rgb ) < colorBlendOp > ( dstColorBlendFactor * oldColor.rgb );
		finalColor.a = ( srcAlphaBlendFactor * newColor.a ) < alphaBlendOp > ( dstAlphaBlendFactor * oldColor.a );
	}
	else {
		 finalColor = newColor;
	}
	finalColor = finalColor & colorWriteMask;
	*/
	

	//auto bindingDescription = GetBindingDescription();
	//auto attributeDescriptions = GetAttributeDescriptions();
	//VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	//vertexInputInfo.vertexBindingDescriptionCount = 1;
	//vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>( attributeDescriptions.size() );
	//vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	//vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


	// pipeline layout
	
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1; 
	pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout; 
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if(vkCreatePipelineLayout( m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create pipeline layout!" );
	}

	// pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending; // Optional
	pipelineInfo.pDynamicState = & dynamicState; // Optional
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = m_renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if(vkCreateGraphicsPipelines( m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create graphics pipeline!" );
	}
}

void RenderContext_vulkan::CreateFrameBuffers()
{
	m_swapChainFramebuffers.resize( m_swapChainImageViews.size() );
	for( size_t i = 0; i < m_swapChainImageViews.size(); i++ )
	{
		VkImageView attachments[] = {
			m_swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_swapChainExtent.width;
		framebufferInfo.height = m_swapChainExtent.height;
		framebufferInfo.layers = 1;

		if( vkCreateFramebuffer( m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i] ) != VK_SUCCESS )
		{
			ERROR_AND_DIE( "Failed to create framebuffer!" );
		}
	}
}

void RenderContext_vulkan::CreateCommandPool()
{
	QueueFamilyIndices indices = FindQueueFamilies( m_physicalDevice, m_surface );
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamily.value(); 
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if( vkCreateCommandPool( m_device, &poolInfo, nullptr, &m_commandPool ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create command pool!" );
	}
}

void RenderContext_vulkan::createVertexBuffer()
{
	m_immediateVBO = new VertexBuffer(this, RenderMemoryHint::MEMORY_HINT_GPU);
	size_t bufferSize = sizeof( Vertex_PCU ) * debugDrawData.size();
	m_immediateVBO->Update( debugDrawData.data(), bufferSize, sizeof( Vertex_PCU ) );
	// create staging buffer
	//VkBuffer stageBuffer;
	//VkDeviceMemory stagingBufferMemory;
	//CreateBuffer( m_device, m_physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stageBuffer, stagingBufferMemory );
	//void* data;
	//// driver may not be able to copy the data to the buffer immediately.
	//// 1.VK_MEMORY_PROPERTY_HOST_COHERENT_BIT - the driver will make sure that the data is always in a coherent state
	//// 2.Call vkFlushMappedMemoryRanges after writing to the mapped memory, 
	////		and call vkInvalidateMappedMemoryRanges before reading from the mapped memory
	//vkMapMemory( m_device, stagingBufferMemory, 0, bufferSize, 0, &data );
	//memcpy(data, debugDrawData.data(), (size_t)bufferSize);
	//vkUnmapMemory( m_device, stagingBufferMemory );
	//
	//
	//// create vertex buffer
	//CreateBuffer(m_device, m_physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
	//	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);
	//
	//
	//CopyBuffer(m_device, m_commandPool, m_graphicsQueue, stageBuffer, m_vertexBuffer, bufferSize);
}

void RenderContext_vulkan::CreateIndexBuffer()
{
	m_devIBO = new IndexBuffer(this, RenderMemoryHint::MEMORY_HINT_GPU);
	m_devIBO->Update( debugDrawIndexes );
	//VkDeviceSize bufferSize = sizeof(debugDrawIndexes[0]) * debugDrawIndexes.size();
	//
	//VkBuffer stageBuffer;
	//VkDeviceMemory stagingBufferMemory;
	//CreateBuffer( m_device, m_physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stageBuffer, stagingBufferMemory );
	//
	//void* data;
	//vkMapMemory( m_device, stagingBufferMemory, 0, bufferSize, 0, &data );
	//memcpy(data, debugDrawIndexes.data(), (size_t)bufferSize);
	//vkUnmapMemory( m_device, stagingBufferMemory );
	//
	//CreateBuffer(m_device, m_physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
	//	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_lastBoundIBO, m_indexBufferMemory);
	//
	//CopyBuffer(m_device, m_commandPool, m_graphicsQueue, stageBuffer, m_lastBoundIBO, bufferSize);
	//vkDestroyBuffer( m_device, stageBuffer, nullptr );
	//vkFreeMemory( m_device, stagingBufferMemory, nullptr );
}

void RenderContext_vulkan::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof( UniformBufferObject );
	m_uniformBuffers.resize( MAX_FRAMES_IN_FLIGHT );


	for( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
	{
		m_uniformBuffers[i]	= new RenderBuffer( "Uniform Buffer", this, RenderBufferUsageBit::UNIFORM_BUFFER_BIT, RenderMemoryHint::MEMORY_HINT_DYNAMIC);
		m_uniformBuffers[i]->Update(&m_ubo, sizeof(m_ubo), sizeof(m_ubo));
	}
	CreateDescriptorSetLayout();
	CreateDescriptorPool();
	// bind uniform buffer here
	CreateDescriptorSets();
}

void RenderContext_vulkan::CreateDescriptorPool()
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>( MAX_FRAMES_IN_FLIGHT );

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>( MAX_FRAMES_IN_FLIGHT );

	if( vkCreateDescriptorPool( m_device, &poolInfo, nullptr, &m_descriptorPool ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to create descriptor pool!" );
	}
}

void RenderContext_vulkan::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts( MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout );
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>( MAX_FRAMES_IN_FLIGHT );
	allocInfo.pSetLayouts = layouts.data();

	m_descriptorSets.resize( MAX_FRAMES_IN_FLIGHT );

	if( vkAllocateDescriptorSets( m_device, &allocInfo, m_descriptorSets.data() ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to allocate descriptor sets!" );
	}

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
	{
		VkDescriptorBufferInfo bufferInfo = {};
		VkBuffer uboHandle = (VkBuffer)m_uniformBuffers[i]->m_handle;
		bufferInfo.buffer = uboHandle;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof( UniformBufferObject );

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets( m_device, 1, &descriptorWrite, 0, nullptr );
	}

}

void RenderContext_vulkan::CreateDescriptorSet( VertexBuffer* ubo )
{
	std::vector<VkDescriptorSetLayout> layouts( 1, m_descriptorSetLayout );
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>( 1 );
	allocInfo.pSetLayouts = layouts.data();


	if( vkAllocateDescriptorSets( m_device, &allocInfo, m_descriptorSets.data() ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to allocate descriptor sets!" );
	}
}

void RenderContext_vulkan::CreateCommandBuffers()
{
	m_commandBuffers.resize( m_swapChainFramebuffers.size() );
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

	if( vkAllocateCommandBuffers( m_device, &allocInfo, m_commandBuffers.data() ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to allocate command buffer!" );
	}
}

void RenderContext_vulkan::RecordCommandBuffer( VkCommandBuffer commandBuffer, uint32_t imageIndex )
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if( vkBeginCommandBuffer( commandBuffer, &beginInfo) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to begin recording command buffer!" );
	}

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass;
	renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_swapChainExtent;
	VkClearValue clearColor = { { {0.0f, 0.0f, 0.0f, 1.0f} } };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline );

	// viewport and scissor
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_swapChainExtent.width;
	viewport.height = (float)m_swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport( commandBuffer, 0, 1, &viewport );

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChainExtent;
	vkCmdSetScissor( commandBuffer, 0, 1, &scissor );
	BindVertexBuffer(m_immediateVBO);
	BindIndexBuffer( m_devIBO );
	

	vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[m_currentFrame], 0, nullptr );
	// vkCmdDraw( commandBuffer, static_cast<uint32_t>(debugDrawData.size() ), 1, 0, 0); // draw a triangle
	vkCmdDrawIndexed( commandBuffer, static_cast<uint32_t>(debugDrawIndexes.size()), 1, 0, 0, 0); // draw a triangle
	vkCmdEndRenderPass( commandBuffer );
	if( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
	{
		ERROR_AND_DIE( "Failed to record command buffer!" );
	}

}

void RenderContext_vulkan::CreateSyncObjects()
{
	m_imageAvailableSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
	m_renderFinishedSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
	m_inFlightFences.resize( MAX_FRAMES_IN_FLIGHT );

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	// fence is signaled when creating. The first frame will not wait cause the fence is signaled already 
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; 

	for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ){
		if(vkCreateSemaphore( m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i] ) != VK_SUCCESS ||
			vkCreateSemaphore( m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i] ) != VK_SUCCESS ||
			vkCreateFence( m_device, &fenceInfo, nullptr, &m_inFlightFences[i] ) != VK_SUCCESS )
		{
			ERROR_AND_DIE( "Failed to create synchronization objects!" );
		}
	}
}

void RenderContext_vulkan::RecreateSwapChain( )
{
	vkDeviceWaitIdle( m_device ); // wait for the device to finish all operations

	ShutDownSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateFrameBuffers();
}

void RenderContext_vulkan::ShutDownSwapChain()
{
	for(size_t i = 0; i < m_swapChainFramebuffers.size(); i++ )
	{
		vkDestroyFramebuffer( m_device, m_swapChainFramebuffers[i], nullptr );
	}

	for(size_t i = 0; i < m_swapChainImageViews.size(); i++ )
	{
		vkDestroyImageView( m_device, m_swapChainImageViews[i], nullptr );
	}

	vkDestroySwapchainKHR( m_device, m_VkSwapChain, nullptr );
}

void RenderContext_vulkan::UpdateUniformBuffer( uint32_t currentImage )
{
	m_uniformBuffers[currentImage]->Update(&m_ubo, sizeof(m_ubo), sizeof(m_ubo) );
}




