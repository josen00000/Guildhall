﻿#pragma once
#include <map>
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Mat44.hpp"

class BitmapFont;
class Camera;
class GPUMesh;
class IndexBuffer;
class RenderBuffer;
class Sampler;
class SwapChain;
class Shader;
class Texture;
class VertexBuffer;
class Window;

struct AABB2;
struct ID3D11BlendState;
struct ID3D11Buffer;
struct ID3D11DepthStencilState;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct LineSegment2;

typedef std::map<std::string, Shader*>::iterator ShaderMapIterator;

enum BlendMode
{
	BLEND_ALPHA,
	BLEND_ADDITIVE,
	BLEND_OPAQUE,
	NUM_BLENDMODE
};

enum BufferSlot {
	UBO_FRAME_SLOT	= 0,
	UBO_CAMERA_SLOT,
	UBO_MODEL_SLOT
};

// 	D3D11_COMPARISON_NEVER	= 1,
// 	D3D11_COMPARISON_LESS	= 2,
// 	D3D11_COMPARISON_EQUAL	= 3,
// 	D3D11_COMPARISON_LESS_EQUAL	= 4,
// 	D3D11_COMPARISON_GREATER	= 5,
// 	D3D11_COMPARISON_NOT_EQUAL	= 6,
// 	D3D11_COMPARISON_GREATER_EQUAL	= 7,
// 	D3D11_COMPARISON_ALWAYS	= 8
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

// data used in shader
struct time_data_t {
	float system_time;
	float system_delta_time;
	float padding[2];
};

struct model_t {
	Mat44 model;
};

struct camera_ortho_t {
	Mat44 projection;
	Mat44 view;
};

class RenderContext{
public :
	RenderContext(){}
	~RenderContext(){}

public:
	void StartUp( Window* window );
	void Shutdown();
	void BeginView(){}	// TODO Any use?
	void BeginFrame();
	void EndFrame();
	
	
	void BeginCamera( Camera& camera);
	void EndCamera();
	void ClearTargetView( Texture* output, const Rgba8& clearColor ); // TODO: Change name to clear target target;


	// IA
	void UpdateLayoutIfNeeded(); // private
	
	// Bind
	void BindTexture( Texture* texture);
	void BindSampler( const Sampler* sampler );
	void BindShader( Shader* shader );
	void BindShader( const char* fileName );
	void BindVertexBuffer( VertexBuffer* vbo );
	void BindIndexBuffer( IndexBuffer* ibo );
	void BindUniformBuffer( uint slot, RenderBuffer* ubo ); // ubo - uniform buffer object
	
	// Set State	
	void SetBlendMode( BlendMode blendMode );
	void SetModelMatrix( Mat44 model );
	void EnableDepth( DepthCompareFunc func, bool writeDepthOnPass );
	void DisableDepth();
	bool CheckDepthStencilState( DepthCompareFunc func, bool writeDepthOnPass );

	// Draw
	void Draw( int numVertexes, int vertexOffset = 0 );
	void DrawIndexed( int indexCount, int indexOffset = 0, int vertexOffset = 0 );
	void DrawMesh( GPUMesh* mesh );

	void DrawVertexVector( std::vector<Vertex_PCU>& vertices );
	void DrawVertexArray( int vertexNum, Vertex_PCU* vertexArray );
	void DrawAABB2D( const AABB2& bounds, const Rgba8& tint );
	void DrawLine( const Vec2& startPoint, const Vec2&endPoint, const float thick, const Rgba8& lineColor );
	void DrawLine( const LineSegment2& lineSeg, float thick, const Rgba8& lineColor );
	void DrawCircle( Vec3 center, float radiu, float thick, const Rgba8& circleColor );
	void DrawFilledCircle( Vec3 center, float radiu, const Rgba8& filledColor );

	// Create
	// Database( resource management )
	Texture* GetSwapChainBackBuffer();
	BitmapFont* CreateOrGetBitmapFontFromFile(const char* fontName, const char* fontFilePath);
	Texture* CreateOrGetTextureFromFile(const char* imageFilePath);
	Texture* CreateTextureFromColor( Rgba8 color );
	Shader* GetOrCreateShader( char const* fileName );
	void AddTexture( Texture* tex );


private:
	void			CreateBlendState();
	BitmapFont*		CheckBitmapFontExist( const char* fontName ) const;
	BitmapFont*		CreateBitmapFontFromFile(const char* fontName, const char* fontFilePath);
	Texture*		CreateTextureFromFile(const char* imageFilePath);
	Texture*		CheckTextureExist(const char* imageFilePath) const;
	void			CleanTextures();
	void			CleanShaders();

public:
	ID3D11DeviceContext*	m_context	= nullptr; // how we issue command
	ID3D11Device*			m_device	= nullptr; // reference to gpu
	SwapChain*				m_swapChain	= nullptr;

private:
	void UpdateFrameTime( float deltaSeconds );
	bool m_shaderHasChanged = false;
	std::vector <Texture*> m_textureList;
	std::map<std::string, Texture*>			m_loadedTextures;
	std::map<std::string, BitmapFont*>		m_loadedFonts;
	std::map<std::string, Shader*> m_shaders;

	Sampler*	m_defaultSampler = nullptr;


	Camera*		m_currentCamera	= nullptr;
	Shader*		m_currentShader = nullptr;
	Shader*		m_defaultShader	= nullptr;

	ID3D11Buffer*	m_lastBoundIBO	= nullptr;	
	ID3D11Buffer*	m_lastBoundVBO	= nullptr;
	VertexBuffer*	m_immediateVBO	= nullptr;
	RenderBuffer*	m_frameUBO		= nullptr;
	RenderBuffer*	m_modelUBO		= nullptr;

// 	buffer_attribute_t* m_currentLayout		= nullptr;
// 	buffer_attribute_t* m_previousLayout	= nullptr;

	Texture* m_texDefaultColor = nullptr;

	// states
	ID3D11BlendState* m_additiveBlendState	= nullptr;
	ID3D11BlendState* m_alphaBlendState		= nullptr;
	ID3D11BlendState* m_opaqueBlendState	= nullptr;

	ID3D11DepthStencilState* m_currentDepthStencilState = nullptr;
};
