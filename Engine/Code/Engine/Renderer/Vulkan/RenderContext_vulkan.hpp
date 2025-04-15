#pragma once

#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Vulkan/VulkanCommon.hpp"

//-----------------------------------------------------------------------------------------------
// TODO:	
//		1.	vkallocatememory should not be called for every buffer
//			The maximum number of simultaneous memory allocations is limited by the maxMemoryAllocationCount physical device limit,
//			The right way to allocate memory for a large number of objects at the same time is to create a custom allocator 
//			that splits up a single allocation among many different objects by using the offset parameters that we've seen in many functions.
//			You can either implement such an allocator yourself, or use the VulkanMemoryAllocator library provided by the GPUOpen initiative.
//		2.	The previous chapter already mentioned that you should allocate multiple resources like buffers from a single memory allocation, 
//			but in fact you should go a step further. Driver developers recommend that you also store multiple buffers, like the vertex and index buffer, 
//			into a single VkBuffer and use offsets in commands like vkCmdBindVertexBuffers. The advantage is that your data is more cache friendly in that case, because it's closer together.
//			It is even possible to reuse the same chunk of memory for multiple resources if they are not used during the same render operations, 
//			provided that their data is refreshed, of course. This is known as aliasing and some Vulkan functions have explicit flags to specify that you want to do this.

class VertexBuffer;


class RenderContext_vulkan : public RenderContext
{
public:
	RenderContext_vulkan();
	~RenderContext_vulkan();

public:

	virtual void StartUp( Window* window );
	virtual void ShutDown();
	virtual void BeginFrame();
	virtual void EndFrame();
	virtual void BeginCamera( Camera* camera, Convention convention = X_RIGHT_Y_UP_Z_BACKWARD );
	virtual void EndCamera();
	virtual void ClearState();
	virtual void CreateRenderBuffer( RenderBuffer& buffer ) override;
	virtual void UpdateRenderBuffer( RenderBuffer& buffer, void const* data, size_t dataByteSize, size_t elementByteSize ) override;
	virtual void CleanUpRenderBuffer( RenderBuffer& buffer ) override;


	virtual void EnableDepth( DepthCompareFunc func, bool writeDepthOnPass );
	virtual void DisableDepth();

	// mutators
	virtual void SetDiffuseTexture( Texture* texture, int index = 0 );
	virtual void SetTintColor( const Rgba8& tint )  ;

	// raster state
	virtual void SetRasterCullMode( RasterCullMode mode )  ;
	virtual void SetRasterFillMode( RasterFillMode mode )  ;
	virtual void SetFrontFaceWindOrder( RasterWindOrder order )  ;


	// Draw
	virtual void Draw( int numVertexes, int vertexOffset = 0 )  ;
	virtual void DrawMesh( GPUMesh* mesh )  ;
	virtual void DrawIndexed( int indexCount, int indexOffset = 0, int vertexOffset = 0 )  ;
	virtual void DrawIndexedVertexVector( const std::vector<Vertex_PCU>& vertices, const std::vector<uint>& indexes )  ;
	virtual void DrawVertexVector( const std::vector<Vertex_PCU>& vertices )  ;
	virtual void DrawVertexArray( int vertexNum, Vertex_PCU* vertexArray )  ;
	virtual void DrawLine( const Vec2& startPoint, const Vec2& endPoint, const float thick, const Rgba8& lineColor )  ;
	virtual void DrawAABB2D( const AABB2& bounds, const Rgba8& tint, const Vec2& uvMin = Vec2::ZERO, const Vec2& mvMax = Vec2::ONE )  ;
	virtual void DrawCircle( Vec3 center, float radius, float thick, const Rgba8& circleColor )  ;

private:
	bool CheckValidationLayersSupport( const std::vector<const char*>& validationLayers );
	void GetAllExtensions();
	void CreateInstance();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSurface();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateDescriptorSetLayout();
	void CreateGraphicsPipeline();
	void CreateFrameBuffers();
	void CreateCommandPool();
	void createVertexBuffer();
	void CreateIndexBuffer();
	void CreateUniformBuffers();
	void CreateDescriptorPool();
	void CreateDescriptorSets();
	void CreateCommandBuffers();
	void RecordCommandBuffer(VkCommandBuffer commandBuffer,  uint32_t imageIndex );
	void CreateSyncObjects();
	void RecreateSwapChain();
	void ShutDownSwapChain();
	void UpdateUniformBuffer( uint32_t currentImage );

private:
	uint32_t m_currentFrame = 0;
	Window* m_window = nullptr;
	VkInstance m_instance = NULL;
	VkDebugUtilsMessengerEXT m_debugMessenger = NULL;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	VkQueue m_presentQueue = VK_NULL_HANDLE;
	VkSwapchainKHR m_VkSwapChain = VK_NULL_HANDLE; // TODO: Need to integrate with swapchain
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	std::vector<VkImageView> m_swapChainImageViews;
	VkShaderModule m_vertShaderModule = VK_NULL_HANDLE;
	VkShaderModule m_fragShaderModule = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> m_swapChainFramebuffers;
	VkCommandPool m_commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_commandBuffers;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences; // is rendering finished
	
	VertexBuffer* m_vertexBuffer = nullptr;
	VkBuffer m_indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory m_indexBufferMemory = VK_NULL_HANDLE;
	std::vector<VkBuffer> m_uniformBuffers;
	std::vector<VkDeviceMemory> m_uniformBuffersMemory;
	std::vector<void*> m_uniformBufferMapped;
	VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> m_descriptorSets;
};