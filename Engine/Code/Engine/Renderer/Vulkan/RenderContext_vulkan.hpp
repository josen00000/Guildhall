#pragma once

#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Vulkan/VulkanCommon.hpp"


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
	void CreateGraphicsPipeline();
	void CreateFrameBuffers();
	void CreateCommandPool();
	void CreateCommandBuffers();
	void RecordCommandBuffer(VkCommandBuffer commandBuffer,  uint32_t imageIndex );
	void CreateSyncObjects();
	void RecreateSwapChain();
	void ShutDownSwapChain();

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
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> m_swapChainFramebuffers;
	VkCommandPool m_commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_commandBuffers;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences; // is rendering finished
};