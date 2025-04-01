#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/Camera.hpp"

class Texture;
class Clock;
class GPUMesh;
class SwapChain;

enum RenderContextType
{
	RENDER_CONTEXT_TYPE_D3D11,
	RENDER_CONTEXT_TYPE_OPENGL,
	RENDER_CONTEXT_TYPE_VULKAN,
	RENDER_CONTEXT_TYPE_NONE
};

enum DepthCompareFunc {
	COMPARE_DEPTH_INVALID = 0,
	COMPARE_DEPTH_NEVER,
	COMPARE_DEPTH_LESS,
	COMPARE_DEPTH_EQUAL,
	COMPARE_DEPTH_LESS_EQUAL,
	COMPARE_DEPTH_GREATER,
	COMPARE_DEPTH_NOT_EQUAL,
	COMPARE_DEPTH_GREATER_EQUAL,
	COMPARE_DEPTH_ALWAYS,
	NUM_COMPARE_DEPTH_FUNC
};

// raster state
enum RasterCullMode {
	RASTER_CULL_NONE,
	RASTER_CULL_FRONT,
	RASTER_CULL_BACK,
};

enum RasterFillMode {
	RASTER_FILL_WIREFRAME,
	RASTER_FILL_SOLID,
};

enum RasterWindOrder {
	FRONT_CLOCKWISE,
	FRONT_COUNTER_CLOCKWISE
};
//-----------------------------------------------------------------------------------------------

// shader data
struct model_t {
	Mat44 modelMat;
	float specularFactor;
	float specularPow;
	float padding[2];
};
//-----------------------------------------------------------------------------------------------


class RenderContext{
public:
	RenderContext( RenderContextType type);
	virtual ~RenderContext(){};
	static RenderContext* s_renderContext;

public:
	virtual void StartUp(Window* window ) = 0;
	virtual void ShutDown() = 0;
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void BeginCamera( Camera* camera, Convention convention = X_RIGHT_Y_UP_Z_BACKWARD ) = 0;
	virtual void EndCamera() = 0;
	virtual void ClearState() = 0;

	// Accessors
	RenderContextType GetRenderContextType() const { return m_type; }
	Texture* GetSwapChainBackBuffer();
	Clock* GetClock() const { return m_clock; }

	virtual void EnableDepth( DepthCompareFunc func, bool writeDepthOnPass ) = 0;
	virtual void DisableDepth() = 0;

	// mutators
	virtual void SetDiffuseTexture( Texture* texture, int index = 0 ) = 0;
	virtual void SetTintColor( const Rgba8& tint ) = 0;
	void SetModelMatrix( Mat44 model );

	// raster state
	virtual void SetRasterCullMode( RasterCullMode mode ) = 0;
	virtual void SetRasterFillMode( RasterFillMode mode ) = 0;
	virtual void SetFrontFaceWindOrder( RasterWindOrder order ) = 0;
	//virtual void SetModelMatrix( const Mat44& model ) = 0;


	// Draw
	virtual void Draw( int numVertexes, int vertexOffset = 0 ) = 0;
	virtual void DrawMesh( GPUMesh* mesh ) = 0;
	virtual void DrawIndexed( int indexCount, int indexOffset = 0, int vertexOffset = 0 ) = 0;
	virtual void DrawIndexedVertexVector( const std::vector<Vertex_PCU>& vertices, const std::vector<uint>& indexes ) = 0;
	virtual void DrawVertexVector( const std::vector<Vertex_PCU>& vertices ) = 0;
	virtual void DrawVertexArray( int vertexNum, Vertex_PCU* vertexArray ) = 0;
	virtual void DrawLine( const Vec2& startPoint, const Vec2& endPoint, const float thick, const Rgba8& lineColor ) = 0;
	virtual void DrawAABB2D( const AABB2& bounds, const Rgba8& tint, const Vec2& uvMin = Vec2::ZERO, const Vec2& mvMax = Vec2::ONE ) = 0;
	virtual void DrawCircle( Vec3 center, float radius, float thick, const Rgba8& circleColor ) = 0;

public:

protected:
	RenderContextType m_type = RENDER_CONTEXT_TYPE_NONE;
	SwapChain* m_swapChain = nullptr;
	Clock* m_clock = nullptr;
	model_t m_model;
	bool m_modelHasChanged = false;
};