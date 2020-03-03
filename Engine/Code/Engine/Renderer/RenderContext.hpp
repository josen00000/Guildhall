#pragma once
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
struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11BlendState;

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
	void BeginView();
	
	void UpdateFrameTime( float deltaSeconds );
	void BeginCamera( Camera& camera);
	void EndCamera(const Camera& camera);
	
	void BeginFrame();
	void EndFrame();
	void ClearScreen( Texture* output, const Rgba8& clearColor ); // TODO: Change name to clear target target;
	void SetOrthoView(const AABB2& box);

	//IA
	void UpdateLayoutIfNeeded();
	
	// Bind
	void BindTexture( Texture* texture);
	void BindSampler( const Sampler* sampler );
	void BindShader( Shader* shader );
	void BindShader( const char* fileName );
	void BindVertexBuffer( VertexBuffer* vbo );
	void BindIndexBuffer( IndexBuffer* ibo );
	void BindUniformBuffer( uint slot, RenderBuffer* ubo ); // ubo - uniform buffer object

	// Draw
	void Draw( int numVertexes, int vertexOffset = 0 );
	void DrawIndexed( int indexCount, int indexOffset = 0, int vertexOffset = 0 );
	void DrawMesh( GPUMesh* mesh );
	void DrawVertexVector( std::vector<Vertex_PCU>& vertices );
	void DrawVertexArray( int vertexNum, Vertex_PCU* vertexArray );
	void DrawAABB2D( const AABB2& bounds, const Rgba8& tint );
	void DrawLine( const Vec2& startPoint, const Vec2&endPoint, const float thick, const Rgba8& lineColor );
	void DrawCircle( Vec3 center, float radiu, float thick, Rgba8& circleColor );

	// Create
	Texture* CreateOrGetTextureFromFile(const char* imageFilePath);
	Texture* CreateTextureFromColor( Rgba8 color );
	BitmapFont* CreateOrGetBitmapFontFromFile(const char* fontName, const char* fontFilePath);
	Shader* GetOrCreateShader( char const* fileName );

	// Mutator	
	void SetBlendMode(BlendMode blendMode);
	void SetModelMatrix( Mat44 model );

private:
	Texture*		CreateTextureFromFile(const char* imageFilePath);
	BitmapFont*		CreateBitmapFontFromFile(const char* fontName, const char* fontFilePath);
	void			CreateBlendState();
	Texture*		CheckTextureExist(const char* imageFilePath) const;
	BitmapFont*		CheckBitmapFontExist( const char* fontName ) const;
	void			CleanTextures();

public:
	ID3D11DeviceContext*	m_context	= nullptr; // how we issue command
	ID3D11Device*			m_device	= nullptr; // reference to gpu
	SwapChain*				m_swapChain	= nullptr;

private:
	bool m_shaderHasChanged = false;
	std::vector <Texture*> m_textureList;
	std::map<std::string, Texture*>			m_loadedTextures;
	std::map<std::string, BitmapFont*>		m_loadedFonts;

	Shader*			m_currentShader = nullptr;
	Shader*			m_defaultShader	= nullptr;
	std::map<std::string, Shader*> m_shaders;

	ID3D11Buffer*	m_lastBoundIBO	= nullptr;	
	ID3D11Buffer*	m_lastBoundVBO	= nullptr;
	VertexBuffer*	m_immediateVBO	= nullptr;
	RenderBuffer*	m_frameUBO		= nullptr;
	RenderBuffer*	m_modelUBO		= nullptr;

// 	buffer_attribute_t* m_currentLayout		= nullptr;
// 	buffer_attribute_t* m_previousLayout	= nullptr;

	Sampler* m_defaultSampler = nullptr;
	Texture* m_texDefaultColor = nullptr;

	ID3D11BlendState* m_additiveBlendState	= nullptr;
	ID3D11BlendState* m_alphaBlendState		= nullptr;
	ID3D11BlendState* m_opaqueBlendState	= nullptr;
};
