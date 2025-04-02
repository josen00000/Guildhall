#pragma once
#define VK_USE_PLATFORM_WIN32_KHR

#include "Engine/Renderer/RenderContext.hpp"
#include <vulkan/vulkan.h>


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

private:
	VkInstance m_instance = NULL;
	VkDebugUtilsMessengerEXT m_debugMessenger = NULL;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;
	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
};