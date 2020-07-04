#pragma once
#include <map>
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

class BitmapFont;
class Camera;
class Clock;
class GPUMesh;
class Image;
class IndexBuffer;
class RenderBuffer;
class Sampler;
class Shader;
class ShaderState;
class SwapChain;
class Texture;
class VertexBuffer;
class Window;
class Material;

struct AABB2;
struct Cylinder3;
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
typedef Vec4 color_intensity;



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
	UBO_SCENE_DATA_SLOT,
	UBO_MATERIAL_SLOT,
	UBO_DISSOLVE_SLOT,
};

enum TextureSlot: uint{
	TEXTURE_DIFFUSE_SLOT0 = 0,
	TEXTURE_DIFFUSE_SLOT1,
	TEXTURE_DIFFUSE_SLOT2,
	TEXTURE_DIFFUSE_SLOT3,
	TEXTURE_NORMAL_SLOT0,
	TEXTURE_NORMAL_SLOT1,
	TEXTURE_NORMAL_SLOT2,
	TEXTURE_NORMAL_SLOT3,
	TEXTURE_USER_SLOT1 = 8
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

enum TexureCubeSide {
	TEXCUBE_FRONT,
	TEXCUBE_BACK,
	TEXCUBE_LEFT,
	TEXCUBE_RIGHT,
	TEXCUBE_TOP,
	TEXCUBE_BOTTOM,
	TEXCUBE_NUM 
};

// data used in shader
struct time_data_t {
	float system_time;
	float system_delta_time;
	float padding[2];
};

struct model_t {
	Mat44 modelMat;
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

struct light_t { // _t means type
	Vec3 position = Vec3::ZERO;
	float pad0;
	color_intensity color = Vec4::ONE;
	Vec3 diffuseAttenuation = Vec3( 0.0f, 0.0f, 1.0f );
	float cos_inner_half_angle = -1.0f;
	Vec3 specularAttenuation = Vec3( 0.0f, 0.0f, 1.0f );
	float cos_outer_half_angle = -1.0f;
	Vec3 direction = Vec3( 1.f, 0.f, 0.f );
	float direction_factor = 0.0f; // what is factor choose between direction light or point light
									// bool direction light or point light
};


struct scene_data_t {
	color_intensity ambient;
	color_intensity emissive;
	
	light_t lights[MAX_LIGHTS_NUM];

	float diffuse_factor = 1.0f;
	float emissive_factor = 1.0f;
	float pad[2];

	float fog_near_distance = 0.0f;
	Vec3 fog_near_color = Vec3( 1.f, 1.f, 1.f );
	float fog_far_distance = 0.0f;
	Vec3 fog_far_color = Vec3( 1.f, 1.f, 1.f );
};

struct dissolve_data_t {
	Vec3 burnStartColor = Vec3( 1.f, 0.f, 0.f );
	float burnEdgeWidth = 0.0f;
	Vec3 burnEndColor = Vec3( 0.f, 0.f, 0.f );
	float burnAmount = 0.0f;
};

struct project_data_t {
	Mat44 world_to_clip = Mat44();
	Vec3 pos = Vec3::ZERO;
	float power = 0.0f;
};

struct parallax_data_t {
	int count;
	float depth;
	float pad[2];
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
	
	void BeginCamera( Camera* camera, Convention convention );
	void EndCamera();

	void ClearState();
	void ClearTargetView( Texture* output, const Rgba8& clearColor ); // TODO: Change name to clear target target;
	
	// new for render target
	Texture* CreateRenderTarget( IntVec2 texSize );
	void CopyTexture( Texture* dst, Texture* src );
	void ApplyEffect( Texture* dst, Texture* src, Material* mat );
	void StartEffect( Texture* dst, Texture* src, Shader* shader, RenderBuffer* ubo );
	void StartBloomEffect( Texture* dst, Texture* src, Texture* color, Shader* shader, RenderBuffer* ubo );
	void EndEffect();

	Texture* AcquireRenderTargetMatching( Texture* texture );
	void ReleaseRenderTarget( Texture* tex );
	int GetTotalTexturePoolCount() const { return m_totalRenderTargetMade; }
	int GetTexturePoolFreeCount() const { return (int)m_renderTargetPool.size(); }
	
	// Bind
	void SetDiffuseTexture( Texture* texture, int index = 0 );
	void SetNormalTexture( Texture* texture, int index = 0 );
	void SetMaterialTexture( Texture* texture, int index = 0 );
	void SetFog( float fogFarDist, float fogNearDist, Rgba8 fogFarColor, Rgba8 fogNearColor );
	void DisableFog();
	void BindSampler( const Sampler* sampler );
	void BindShader( Shader* shader );
	void BindShader( const char* fileName );
	void BindShader( std::string fileName );
	void BindShaderState( ShaderState* state );
	void BindRasterState();
	void BindVertexBuffer( VertexBuffer* vbo );
	void BindIndexBuffer( IndexBuffer* ibo );
	void BindUniformBuffer( uint slot, RenderBuffer* ubo ); // ubo - uniform buffer object
	void BindMaterial( Material* matl );
	
	// Set State	
	void SetBlendMode( BlendMode blendMode );
	void SetModelMatrix( Mat44 model );
	void SetSpecularFactor( float factor );
	void SetSpecularPow( float pow );
	void SetModelAndSpecular( Mat44 model, float factor, float pow );
	void SetMaterialBuffer( RenderBuffer* ubo );
	void SetDissolveData( Vec3 startColor, Vec3 endColor, float width, float amount );
	void SetTintColor( Vec4 color );
	void SetTintColor( Rgba8 color );
	void EnableDepth( DepthCompareFunc func, bool writeDepthOnPass );
	void DisableDepth();
	void SetDiffuseAttenuation( int lightIndex, Vec3 atten );
	void SetSpecularAttenuation( int lightIndex, Vec3 atten );
	bool CheckDepthStencilState( DepthCompareFunc func, bool writeDepthOnPass );


	// raster state
	
	void SetCullMode( RasterCullMode mode );
	void SetFillMode( RasterFillMode mode );
	void SetFrontFaceWindOrder( RasterWindOrder order );

	// Draw
	void Draw( int numVertexes, int vertexOffset = 0 );
	void DrawIndexed( int indexCount, int indexOffset = 0, int vertexOffset = 0 );
	void DrawMesh( GPUMesh* mesh );

	void DrawVertexVector( const std::vector<Vertex_PCU>& vertices );
	void DrawVertexArray( int vertexNum, Vertex_PCU* vertexArray );
	void DrawAABB2D( const AABB2& bounds, const Rgba8& tint );
	void DrawLine( const Vec2& startPoint, const Vec2&endPoint, const float thick, const Rgba8& lineColor );
	void DrawLine( const LineSegment2& lineSeg, float thick, const Rgba8& lineColor );
	void DrawLineWithHeight( const LineSegment2& lineSeg, float height, float thick, const Rgba8& lineColor );
	void DrawCircle( Vec3 center, float radiu, float thick, const Rgba8& circleColor );
	void DrawFilledCircle( Vec3 center, float radiu, const Rgba8& filledColor );

	// 3d Draw
	void DrawCylinder( const Cylinder3& cylinder, int level, const Rgba8& tintColor );
	
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

	void EnableLight( int index, const light_t& lightInfo );
	void EnablePointLight( int index, Vec3 position, Rgba8 color, float intensity, Vec3 attenuation );
	void EnableDirectionLight( int index, Vec3 position, Rgba8 color, float intensity, Vec3 attenuation );
	void EnableSpotLight( int index, Vec3 position, Rgba8 color, float intensity, Vec3 attenuation, Vec3 direction, float halfInnerDegrees, float halfOuterDegrees );
	void DisableLight( int index );


	// end 
	void Prensent();

private:
	void	UpdateLayoutIfNeeded(); 
	void	UpdateSceneIfNeeded();
	void	UpdateModelIfNeeded();
	void	UpdateFrameTime( float deltaSeconds );
	void	CreateBlendState();
	void	BindTexture( Texture* texture, uint slot );
	void	SetAttenuation( int lightIndex, Vec3 atten, AttenuationType type );
	void	InitialLights();

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
	void			CreateCubeMapTexturesFromImages( const Image* src );
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

	int m_totalRenderTargetMade = 0;
private:
	bool m_modelHasChanged			= false;
	bool m_shaderHasChanged			= false;
	bool m_sceneHasChanged			= false;

	int m_currentShaderIndex = 0;

	std::vector<Texture*> m_renderTargetPool;

	std::vector <Texture*> m_textureList;
	std::map<std::string, Texture*>			m_loadedTextures;
	std::map<std::string, BitmapFont*>		m_loadedFonts;
	std::map<std::string, Shader*>			m_shaders;

	// default
	Camera*		m_currentCamera		= nullptr;
	Camera*		m_effectCamera		= nullptr;
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
	RenderBuffer*	m_sceneDataUBO		= nullptr;
	RenderBuffer*	m_dissolveUBO		= nullptr;

 	//buffer_attribute_t* m_currentLayout	= nullptr;
 	//buffer_attribute_t* m_previousLayout	= nullptr;

	Texture* m_texDefaultColor = nullptr;
	Texture* m_texDefaultNormal = nullptr;

	// light
	scene_data_t m_sceneData;
	model_t m_model;
	dissolve_data_t m_dissolveData;

	// states
	ID3D11BlendState* m_additiveBlendState	= nullptr;
	ID3D11BlendState* m_alphaBlendState		= nullptr;
	ID3D11BlendState* m_opaqueBlendState	= nullptr;

	ID3D11DepthStencilState* m_currentDepthStencilState = nullptr;

	D3D11_RASTERIZER_DESC* m_defaultRasterStateDesc = nullptr;
	ID3D11RasterizerState* m_rasterState		= nullptr;

};
