#pragma once

#include <vector>
#include "Engine/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine\Core\Time.hpp"
//third party library



#pragma comment( lib, "d3d11.lib" )         // needed a01
#pragma comment( lib, "dxgi.lib" )          // needed a01
#pragma comment( lib, "d3dcompiler.lib" )   // needed when we get to shaders

#define RENDER_DEBUG

void RenderContext::StartUp( Window* window )
{
	IDXGISwapChain* swapchain;
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

#if defined(RENDER_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif	

	// define swapchain
	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	memset( &swapchainDesc, 0, sizeof( swapchainDesc ) );
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;

	// how many back buffers in our chain - we'll double buffer (one we show, one we draw to)
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // on swap, the old buffer is discarded
	swapchainDesc.Flags = 0; // additional flags - see docs.  Used in special cases like for video buffers
	//swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	
	HWND hwnd = (HWND) window->m_hwnd;
	swapchainDesc.OutputWindow = hwnd; // HWND for the window to be used
	swapchainDesc.SampleDesc.Count = 1; // how many samples per pixel (1 so no MSAA)
										 // note, if we're doing MSAA, we'll do it on a secondary target

										 // describe the buffer
	swapchainDesc.Windowed = TRUE;                                    // windowed/full-screen mode
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color RGBA8 color
	swapchainDesc.BufferDesc.Width = window->GetClientWidth();
	swapchainDesc.BufferDesc.Height = window->GetClientHeight();

	// create
	D3D11CreateDeviceAndSwapChain( nullptr, 
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags, // controls the type of device we make,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapchainDesc,
		&swapchain,
		&m_device,
		nullptr,
		&m_context );

	m_swapChain = new SwapChain( this, swapchain );

	m_defaultShader = new Shader( this );
	m_defaultShader->CreateFromFile( "data/Shader/default.hlsl" );
	m_immediateVBO = new VertexBuffer( this, MEMORY_HINT_DYNAMIC );
	m_immediateVBO->m_stride = sizeof(Vertex_PCU);

	m_frameUBO = new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );

}

void RenderContext::BeginView(){

}

void RenderContext::UpdateFrameTime( float deltaSeconds )
{
	time_data_t frameData;
	frameData.system_time = GetCurrentTimeSeconds();
	frameData.system_delta_time = deltaSeconds;

	m_frameUBO->Update( &frameData, sizeof( frameData ), sizeof( frameData ) );
}

void RenderContext::BeginFrame(  )
{
// 	time_data_t frameData;
// 	frameData.system_time = GetCurrentTimeSeconds();
// 	frameData.system_delta_time = deltaSeconds;
// 
// 	m_frameUBO->Update( &frameData, sizeof(frameData), sizeof(frameData) );
}

void RenderContext::EndFrame()
{
	m_swapChain->Present();
}

void RenderContext::Shutdown()
{
	delete m_swapChain;
	delete m_currentShader;
	delete m_immediateVBO;
	delete m_frameUBO;
	m_swapChain		= nullptr;
	m_currentShader = nullptr;
	m_immediateVBO	= nullptr;
	m_frameUBO		= nullptr;

	for( ShaderMapIterator it = m_shaders.begin(); it != m_shaders.end(); ++it ) {
		delete it->second;
		m_shaders.erase(it);
	}

	DX_SAFE_RELEASE(m_device);
	DX_SAFE_RELEASE(m_context);
}


void RenderContext::ClearScreen( Texture* output, const Rgba8& clearColor )
{
	float clearFolats[4];
	clearFolats[0] = (float)clearColor.r / 255.f;
	clearFolats[1] = (float)clearColor.g / 255.f;
	clearFolats[2] = (float)clearColor.b / 255.f;
	clearFolats[3] = (float)clearColor.a / 255.f;

	
	TextureView* backBuffer_rtv = output->GetRenderTargetView();
	ID3D11RenderTargetView* rtv = backBuffer_rtv->GetRTVHandle();
	m_context->ClearRenderTargetView( rtv, clearFolats );

}

void RenderContext::BeginCamera( Camera& camera )
{
#if defined(RENDER_DEBUG)
	m_context->ClearState();
#endif
	Texture* output = camera.GetColorTarget();

	if( output == nullptr ) {
		output = m_swapChain->GetBackBuffer();
	}

	TextureView* view = output->GetRenderTargetView();
	ID3D11RenderTargetView* rtv = view->GetRTVHandle();

	IntVec2 outputSize = output->GetTexelSize();
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX	= 0;
	viewport.TopLeftY	= 0;
	viewport.Width		= output->m_texelSizeCoords.x; //texture->getWidth();		
	viewport.Height		= output->m_texelSizeCoords.y; //texture->GetHeight();		
	viewport.MinDepth	= 0.0f;
	viewport.MaxDepth	= 1.0f;

															// Setup the GPU for a draw
	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_context->RSSetViewports( 1, &viewport );
	m_context->OMSetRenderTargets( 1, &rtv, nullptr ); // TODO begin camera
	
	if( camera.GetShouldClearColor() ) {
		ClearScreen( output, camera.GetClearColor() );
	}
	BindShader( static_cast<Shader*>( nullptr ) );
	m_lastBoundVBO = nullptr;

	RenderBuffer* cameraUBO = camera.GetOrCreateCameraBuffer( this );

	BindUniformBuffer( 0, m_frameUBO );
	camera_ortho_t cameraData;
	cameraData.orthoMax = camera.GetOrthoTopRight();
	cameraData.orthoMin = camera.GetOrthoBottomLeft();
	cameraUBO->Update( &cameraData, sizeof( cameraData ), sizeof( cameraData ) );
	BindUniformBuffer( UBO_CAMERA_SLOT, cameraUBO );

}

void RenderContext::SetOrthoView( const AABB2& box )
{
	UNUSED( box ); 
}

void RenderContext::EndCamera( const Camera& camera )
{
	UNUSED(camera);
}

void RenderContext::BindTexture( const Texture* texture )
{
	UNUSED( texture );
}

void RenderContext::BindShader( Shader* shader )
{
		// Assert_OR_DIE( isdrawing)  DO I have a camera;
	if( shader == nullptr ) {
		m_currentShader = m_defaultShader;
	}
	else {
		m_currentShader = shader;
	}

	m_context->VSSetShader( m_currentShader->m_vertexStage.m_vs, nullptr, 0 );
	m_context->RSSetState( m_currentShader->m_rasterState ); // use defaults
	m_context->PSSetShader( m_currentShader->m_fragmentStage.m_fs, nullptr, 0 );
}

void RenderContext::BindShader( const char* fileName )
{
	Shader* shader = GetOrCreateShader( fileName );
	m_currentShader	 = shader;
}

void RenderContext::BindVertexInput( VertexBuffer* vbo )
{
	ID3D11Buffer* vboHandle = vbo->m_handle;
	UINT stride = (UINT)vbo->m_stride;
	UINT offset = 0;


	if( m_lastBoundVBO != vboHandle ) {
		m_context->IASetVertexBuffers( 0, 1, &vboHandle, &stride, &offset );
		m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		m_lastBoundVBO = vboHandle;
	}
	
}

void RenderContext::Draw( int numVertexes, int vertexOffset /*= 0 */ )
{
	// should be in begin camera
	
	// shader
	// should be in bind shader

	ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout(/* Vertex_PCU::LAYOUT*/); // Vertex 
	m_context->IASetInputLayout( inputLayout );
	m_context->Draw( numVertexes, vertexOffset ); // what is vertexOffset
}

Texture* RenderContext::CreateTextureFromFile( const char* imageFilePath )
{
	UNUSED( imageFilePath );
// 
// 	unsigned int textureID = 0;
// 	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
// 	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
// 	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
// 	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
// 
// 	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
// 	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
// 	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );
// 
// 	//int length = sizeof(*imageData)/sizeof(unsigned int);
// 
// 	// Check if the load was successful
// 	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ));
// 	GUARANTEE_OR_DIE( numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );
// 	// Enable OpenGL texturing
// 	//glEnable( GL_TEXTURE_2D );
// 
// 	// Tell OpenGL that our pixel data is single-byte aligned
// // 	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
// // 
// // 	// Ask OpenGL for an unused texName (ID number) to use for this texture
// // 	glGenTextures( 1, (GLuint*)&textureID );
// // 
// // 	// Tell OpenGL to bind (set) this as the currently active texture
// // 	glBindTexture( GL_TEXTURE_2D, textureID );
// // 
// // 	// Set texture clamp vs. wrap (repeat) default settings
// // 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // GL_CLAMP or GL_REPEAT
// // 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); // GL_CLAMP or GL_REPEAT
// // 
// // 																	// Set magnification (texel > pixel) and minification (texel < pixel) filters
// // 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
// // 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR
// // 
// // 																		// Pick the appropriate OpenGL format (RGB or RGBA) for this texel data
// // 	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
// // 	if( numComponents == 3 )
// // 	{
// // 		bufferFormat = GL_RGB;
// // 	}
// // 	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; technically allows us to translate into a different texture format as we upload to OpenGL
// // 
// // 										  // Upload the image texel data (raw pixels bytes) to OpenGL under this textureID
// // 	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
// // 		GL_TEXTURE_2D,		// Creating this as a 2d texture
// // 		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
// // 		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
// // 		imageTexelSizeX,	// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
// // 		imageTexelSizeY,	// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
// // 		0,					// Border size, in texels (must be 0 or 1, recommend 0)
// // 		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
// // 		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
// // 		imageData );		// Address of the actual pixel data bytes/buffer in system memory
// 
// 							// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
// 	Texture* temTexture = new Texture(textureID,imageFilePath,(float)imageTexelSizeX,(float)imageTexelSizeY);
// 	m_textureList.push_back(temTexture);					
// 	stbi_image_free( imageData );
// 	return temTexture;
	return nullptr;
}

Texture* RenderContext::CreateOrGetTextureFromFile( const char* imageFilePath )
{
	Texture* temTexture = CheckTextureExist(imageFilePath);
	if(temTexture != nullptr){
		return temTexture;
	}
	else{
		temTexture = CreateTextureFromFile(imageFilePath);
		return temTexture;
	}
}

BitmapFont* RenderContext::CreateOrGetBitmapFontFromFile( const char* fontName, const char* fontFilePath )
{
	BitmapFont* temFont = CheckBitmapFontExist(fontName);
	if(temFont != nullptr){ return temFont; }

	temFont = CreateBitmapFontFromFile(fontName, fontFilePath);
	return temFont;
}

Shader* RenderContext::GetOrCreateShader( char const* fileName )
{
	std::string tempName = std::string( fileName );
	ShaderMapIterator it = m_shaders.find( tempName );
	if( it == m_shaders.end() ) {
		return it->second;
	}
	Shader* shader = new Shader( this );
	m_shaders.insert( std::pair<std::string, Shader*>( tempName, shader ));
	return shader;
}

void RenderContext::BindUniformBuffer( uint slot, RenderBuffer* ubo )
{
	ID3D11Buffer* uboHandle = ubo->m_handle;

	m_context->VSSetConstantBuffers( slot, 1, &uboHandle ); // bind to vertex shader
	m_context->PSSetConstantBuffers( slot, 1, &uboHandle ); // bind to pixel shader
}


BitmapFont* RenderContext::CreateBitmapFontFromFile( const char* fontName, const char* fontFilePath )
{
	std::string pngFilePath = fontFilePath;
	pngFilePath.append(".png");
	const char* temPath = pngFilePath.c_str();
	Texture* fontTexture = CreateOrGetTextureFromFile(temPath);
	BitmapFont* tem =  new BitmapFont(fontName, fontTexture);
	return tem;
}

BitmapFont* RenderContext::CheckBitmapFontExist( const char* imageFilePath ) const
{
	auto it =  m_loadedFonts.find(imageFilePath);
	if(it != m_loadedFonts.end() ){
		return it->second;
	}
	return nullptr;
}

Texture* RenderContext::CheckTextureExist(const char* imageFilePath) const
{
	for(int textureIndex=0;textureIndex<m_textureList.size();textureIndex++){
		if(strcmp(imageFilePath,m_textureList[textureIndex]->m_imageFilePath)==0){
			return m_textureList[textureIndex];
		}
	}
	return nullptr;
}

void RenderContext::DrawVertexVector( const std::vector<Vertex_PCU>& vertexArray )
{
	UNUSED( vertexArray );
}

void RenderContext::DrawVertexArray( int vertexNum, Vertex_PCU* vertexes )
{
	// Update a vertex buffer
	size_t elementSize = sizeof(Vertex_PCU);
	size_t bufferByteSize = vertexNum * sizeof(Vertex_PCU);
	m_immediateVBO->Update( vertexes, bufferByteSize, elementSize );

	// Bind
	BindVertexInput( m_immediateVBO );
	
	// Draw
	Draw( vertexNum, 0 );
}

void RenderContext::DrawAABB2D( const AABB2& bounds, const Rgba8& tint )
{
	UNUSED(bounds);
	UNUSED(tint);
// 	Vertex_PCU temAABB2[6] = {
// 		// triangle2
// 		Vertex_PCU(Vec3(bounds.mins,0.f),tint,Vec2(0,0)),
// 		Vertex_PCU(Vec3(bounds.maxs.x,bounds.mins.y,0.f),tint,Vec2(1,0)),
// 		Vertex_PCU(Vec3(bounds.maxs,0.f),tint,Vec2(1,1)),
// 		// triangle2
// 		Vertex_PCU(Vec3(bounds.mins,0.f),tint,Vec2(0,0)),
// 		Vertex_PCU(Vec3(bounds.mins.x,bounds.maxs.y,0.f),tint,Vec2(0,1)),
// 		Vertex_PCU(Vec3(bounds.maxs,0.f),tint,Vec2(1,1)),
// 	};
// 	DrawVertexArray(6,temAABB2);
}

void RenderContext::DrawLine( const Vec2& startPoint, const Vec2& endPoint, const float thick, const Rgba8& lineColor )
{
	UNUSED(startPoint);
	UNUSED(endPoint);
	UNUSED(thick);
	UNUSED(lineColor);
 	float halfThick=thick/2;
 	Vec2 direction=endPoint.operator-( startPoint );
 	direction.SetLength(halfThick);
 	Vec2 tem_position=endPoint+direction;
 	Vec2 rightTop=tem_position.operator+( Vec2(-direction.y,direction.x));
 	Vec2 rightdown=tem_position.operator+(Vec2(direction.y,-direction.x));
 	Vec2 tem_position1=startPoint-direction;
 	Vec2 leftTop=tem_position1.operator+( Vec2( -direction.y, direction.x ) );
 	Vec2 leftdown=tem_position1.operator+(  Vec2( direction.y, -direction.x ));
 	Vec2 tem_uv=Vec2(0.f,0.f);
 	Vertex_PCU line[6]={
 		Vertex_PCU(  Vec3( rightTop.x,rightTop.y,0 ),Rgba8(lineColor.r,lineColor.g,lineColor.b),Vec2( 1, 1 ) ),
 		Vertex_PCU( Vec3(rightdown.x,rightdown.y,0), Rgba8(lineColor.r,lineColor.g,lineColor.b), Vec2( 1, 0 )), 
 		Vertex_PCU( Vec3(leftTop.x,leftTop.y,0), Rgba8(lineColor.r,lineColor.g,lineColor.b), Vec2( 0, 1 ) ),
 		Vertex_PCU(Vec3( leftTop.x,leftTop.y, 0 ), Rgba8(lineColor.r,lineColor.g,lineColor.b), Vec2( 0, 1 ) ),
 		Vertex_PCU(Vec3( leftdown.x,leftdown.y, 0 ), Rgba8(lineColor.r,lineColor.g,lineColor.b), Vec2( 0, 0 ) ),
 		Vertex_PCU(Vec3( rightdown.x,rightdown.y, 0 ), Rgba8(lineColor.r,lineColor.g,lineColor.b), Vec2( 1, 0 ))
 	};
 
	DrawVertexArray(6,line);
}

void RenderContext::DrawCircle( Vec3 center,float radiu,float thick,Rgba8& circleColor )
{
	float degree=0;
	float nextDegree=0;
	for(int i=0;i<64;i++){
		nextDegree=(i+1)*(float)360/64;
		Vec2 startPoint= Vec2();
		startPoint.SetPolarDegrees(degree,radiu);
		startPoint.operator+=(Vec2(center));
		Vec2 endPoint= Vec2();
		endPoint.SetPolarDegrees(nextDegree,radiu);
		endPoint.operator+= ( Vec2(center));
		degree=nextDegree;
		DrawLine(startPoint,endPoint,thick,circleColor);
	}
	

}

void RenderContext::SetBlendMode( BlendMode blendMode )
{
	UNUSED( blendMode );
}

