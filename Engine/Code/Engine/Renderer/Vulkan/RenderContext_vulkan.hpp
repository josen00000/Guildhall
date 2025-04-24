#pragma once

#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Vulkan/VulkanCommon.hpp"
#include <map>
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
//      3.  All of the helper functions that submit commands so far have been set up to execute synchronously by waiting for the queue to become idle.
//			For practical applications it is recommended to combine these operations in a single command buffer and execute them asynchronously for higher throughput,
//			especially the transitions and copy in the createTextureImage function. Try to experiment with this by creating a setupCommandBuffer 
//			that the helper functions record commands into, and add a flushSetupCommands to execute the commands that have been recorded so far. 
//			It's best to do this after the texture mapping works to check if the texture resources are still set up correctly.

class VertexBuffer;
class IndexBuffer;
class RenderBuffer;
class Texture;

typedef std::vector<RenderBuffer*> UniformBuffers;

enum UNIFORM_BUFFER_USAGE
{
	UBO_USAGE_MODEL = 0,
	UBO_USAGE_CAMERA = 1,
	UBO_USAGE_MAX = 2
	//UBO_USAGE_LIGHT = 2,
	//UBO_USAGE_MATERIAL = 3,
	//UBO_USAGE_MAX = 4
};

struct UniformBufferObject{
	Mat44 model;
	Mat44 view;
	Mat44 proj;
};

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

	// buffer
	void BindVertexBuffer( VertexBuffer* buffer );
	void BindIndexBuffer( RenderBuffer* buffer );
	void BindUniformBuffer( RenderBuffer* buffer, uint bindingPoint );

	virtual void EnableDepth( DepthCompareFunc func, bool writeDepthOnPass );
	virtual void DisableDepth();

	// mutators
	virtual void SetDiffuseTexture( Texture* texture, int index = 0 );
	virtual void SetTintColor( const Rgba8& tint )  ;

	// raster state
	virtual void SetRasterCullMode( RasterCullMode mode )  ;
	virtual void SetRasterFillMode( RasterFillMode mode )  ;
	virtual void SetFrontFaceWindOrder( RasterWindOrder order )  ;

	// texture
	virtual Texture* CreateDepthStencilBuffer( int width, int height ) override;
	VkImageView CreateTextureImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags );
	virtual Texture* CreateTextureFromFile( const char* imageFilePath ) override;
	virtual void CreateTextureSampler( Sampler* sampler ) override;
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
	RenderBuffer* CreateUniformBufferWithUsage( UNIFORM_BUFFER_USAGE usage );

	void CreateDescriptorPool();
	void CreateDescriptorSets();
	void CreateDescriptorSet(VertexBuffer* ubo);
	void CreateCommandBuffers();
	void RecordCommandBuffer(VkCommandBuffer commandBuffer,  uint32_t imageIndex );
	void CreateSyncObjects();
	void RecreateSwapChain();
	void ShutDownSwapChain();
	void ShutDownTextures();
	void UpdateUniformBuffer( uint32_t currentImage );
	void TransitionImageLayout( VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout );
	void CopyBufferToImage( VkBuffer buffer, VkImage image, uint32_t width, uint32_t height );
	void CreateDepthResources();

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
	
	// buffer
	VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> m_descriptorSets;
	VkBuffer m_lastBoundIBO = VK_NULL_HANDLE;
	VkBuffer m_lastBoundVBO = VK_NULL_HANDLE;
	VertexBuffer* m_immediateVBO = nullptr;
	IndexBuffer* m_devIBO = nullptr;
	std::vector<UniformBuffers> m_uniformBuffers;
	std::map<Texture*, VkDeviceMemory> m_textures;
	Sampler* m_defaultSampler = nullptr;
	TextureView* m_defaultTextureView = nullptr;

	//depth
	VkImage m_depthImage;
	VkDeviceMemory m_depthImageMemory;
	VkImageView m_depthImageView;
};