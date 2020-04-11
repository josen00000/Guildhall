#pragma once
#include <map>
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

class BitmapFont;
class Clock;
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
struct D3D11_RASTERIZER_DESC;
struct ID3D11BlendState;
struct ID3D11Buffer;
struct ID3D11DepthStencilState;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11RasterizerState;
struct LineSegment2;
struct Vec4;
struct IDXGIDebug;

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
	UBO_MODEL_SLOT,
	UBO_TINT_SLOT,
	UBO_LIGHT_SLOT,
	UBO_AMBIENT_SLOT,
	UBO_ATTENUATION_SLOT
};

enum TextureSlot: uint{
	TEXTURE_DIFFUSE_SLOT = 0,
	TEXTURE_NORMAL_SLOT
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

enum AttenuationType {
	ATTENUATION_DIFFUSE,
	ATTENUATION_SPECULAR
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
	float specularFactor;
	float specularPow;
	float padding[2];
};

struct camera_ortho_t {
	Mat44 projection;
	Mat44 view;
	Vec3 position;
	float pad00;
};

struct tint_color_t {
	float r;
	float g;
	float b;
	float a;
};

struct light_t {
	Vec3 position;
	float pad;
	Vec3 color;
	float intensity;
};

struct attenuation_t {
	Vec3 diffuse;
	float pad0;
	Vec3 specular;
	float pad1;
};

struct ambient_t {
	Vec3 color;
	float intensity;
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
	
	void BeginCamera( Camera* camera);
	void EndCamera();

	void ClearState();
	void ClearTargetView( Texture* output, const Rgba8& clearColor ); // TODO: Change name to clear target target;

	// Bind
	void SetDiffuseTexture( Texture* texture );
	void SetNormalTexture( Texture* texture );
	void BindSampler( const Sampler* sampler );
	void BindShader( Shader* shader );
	void BindShader( const char* fileName );
	void BindShader( std::string fileName );
	void BindRasterState();
	void BindVertexBuffer( VertexBuffer* vbo );
	void BindIndexBuffer( IndexBuffer* ibo );
	void BindUniformBuffer( uint slot, RenderBuffer* ubo ); // ubo - uniform buffer object
	
	// Set State	
	void SetBlendMode( BlendMode blendMode );
	void SetModelMatrix( Mat44 model );
	void SetSpecularFactor( float factor );
	void SetSpecularPow( float pow );
	void SetModelAndSpecular( Mat44 model, float factor, float pow );
	void SetTintColor( Vec4 color );
	void SetTintColor( Rgba8 color );
	void EnableDepth( DepthCompareFunc func, bool writeDepthOnPass );
	void DisableDepth();
	void SetDiffuseAttenuation( Vec3 atten );
	void SetSpecularAttenuation( Vec3 atten );
	bool CheckDepthStencilState( DepthCompareFunc func, bool writeDepthOnPass );


	// raster state
	
	void SetCullMode( RasterCullMode mode );
	void SetFillMode( RasterFillMode mode );
	void SetFrontFaceWindOrder( RasterWindOrder order );

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
	
	// Database( resource management )
	BitmapFont* CreateOrGetBitmapFontFromFile( const char* fontName, const char* fontFilePath );
	Texture* CreateOrGetTextureFromFile( const char* imageFilePath );
	Texture* CreateTextureFromColor( Rgba8 color );
	Texture* CreateTextureFromVec4( Vec4 input );
	Texture* GetSwapChainBackBuffer();
	void AddTexture( Texture* tex );

	Shader* GetOrCreateShader( char const* fileName );
	SwapChain* CreateSwapChain( Window* window, uint flags );
	void createRasterState();

	// lighting
	void SetAmbientColor( Rgba8 color );
	void SetAmbientIntensity( float intensity );
	void SetAmbientLight( Rgba8 color, float intensity ); 
	void DisableAmbient();
	void DebugRenderLight();

	void EnableLight( int index, const light_t& lightInfo );
	void EnablePointLight( int index, Vec3 position, Rgba8 color, float intensity, Vec3 Attenuation );
	void DisableLight( int index );

	// end 
	void Prensent();

private:
	void	UpdateLayoutIfNeeded(); 
	void	UpdateLightIfNeeded(); 
	void	UpdateModelIfNeeded();
	void	UpdateFrameTime( float deltaSeconds );
	void	CreateBlendState();
	void	BindTexture( Texture* texture, uint slot );
	void	SetAttenuation( Vec3 atten, AttenuationType type );

	// debug
	void	CreateDebugModule();
	void	ReportLiveObjects();
	void	DestroyDebugModule();

	// begin camera
	void BeginCameraViewport( IntVec2 viewPortSize );
	void BeginCameraRTVAndViewport( Camera* camera );


	BitmapFont*		CheckBitmapFontExist( const char* fontName ) const;
	BitmapFont*		CreateBitmapFontFromFile(const char* fontName, const char* fontFilePath);
	Texture*		CreateTextureFromFile(const char* imageFilePath);
	Texture*		CheckTextureExist(const char* imageFilePath) const;
	void			CleanTextures();
	void			CleanShaders();

	void	CreateDefaultRasterStateDesc();

public:
	ID3D11DeviceContext*	m_context	= nullptr; // how we issue command
	ID3D11Device*			m_device	= nullptr; // reference to gpu
	SwapChain*				m_swapChain	= nullptr;

	Clock*					m_clock = nullptr;

	// debug mode
	HMODULE m_debugModule = nullptr;
	IDXGIDebug* m_debug = nullptr;

private:
	bool m_modelHasChanged			= false;
	bool m_shaderHasChanged			= false;
	bool m_ambientHasChanged		= false;
	bool m_lightHasChanged			= false;
	bool m_attenuationHasChanged	= false;

	int m_currentShaderIndex = 0;

	std::vector <Texture*> m_textureList;
	std::map<std::string, Texture*>			m_loadedTextures;
	std::map<std::string, BitmapFont*>		m_loadedFonts;
	std::map<std::string, Shader*>			m_shaders;

	// default
	Camera*		m_currentCamera		= nullptr;
	Shader*		m_currentShader		= nullptr;
	Shader*		m_defaultShader		= nullptr;
	Sampler*	m_defaultSampler	= nullptr;

	ID3D11InputLayout* m_previousLayout = nullptr;
	ID3D11InputLayout* m_currentLayout	= nullptr;


	ID3D11Buffer*	m_lastBoundIBO	= nullptr;	
	ID3D11Buffer*	m_lastBoundVBO	= nullptr;
	VertexBuffer*	m_immediateVBO	= nullptr;
	VertexBuffer*	m_currentVBO	= nullptr;
	
	// uniform buffer
	RenderBuffer*	m_frameUBO			= nullptr;
	RenderBuffer*	m_modelUBO			= nullptr;
	RenderBuffer*	m_tintUBO			= nullptr;
	RenderBuffer*	m_lightUBO			= nullptr;
	RenderBuffer*	m_ambientUBO		= nullptr;
	RenderBuffer*	m_attenuationUBO	= nullptr;
	//RenderBuffer*	m_tintUBO		= nullptr;

 	//buffer_attribute_t* m_currentLayout	= nullptr;
 	//buffer_attribute_t* m_previousLayout	= nullptr;

	Texture* m_texDefaultColor = nullptr;
	Texture* m_texDefaultNormal = nullptr;

	// light
	ambient_t m_ambientLight;
	light_t m_light;
	model_t m_model;
	attenuation_t m_attenuation;

	// states
	ID3D11BlendState* m_additiveBlendState	= nullptr;
	ID3D11BlendState* m_alphaBlendState		= nullptr;
	ID3D11BlendState* m_opaqueBlendState	= nullptr;

	ID3D11DepthStencilState* m_currentDepthStencilState = nullptr;

	D3D11_RASTERIZER_DESC* m_defaultRasterStateDesc = nullptr;
	ID3D11RasterizerState* m_rasterState		= nullptr;
};
