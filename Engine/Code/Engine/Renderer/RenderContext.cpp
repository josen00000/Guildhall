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
#include "Engine/Renderer/Sampler.hpp" 
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/SwapChain.hpp"
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
	swapchainDesc.SampleDesc.Count = 1; // how many samples per pixel (1 so no MSAA) note, if we're doing MSAA, we'll do it on a secondary targetdescribe the buffer
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

	m_defaultSampler = new Sampler( this, SAMPLER_POINT );
	m_texDefaultColor = CreateTextureFromColor( Rgba8::WHITE );
	CreateBlendState();
}

void RenderContext::BeginView(){

}

void RenderContext::UpdateFrameTime( float deltaSeconds )
{
	time_data_t frameData;
	frameData.system_time = (float)GetCurrentTimeSeconds();
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
	delete m_defaultShader;
	delete m_immediateVBO;
	delete m_frameUBO;
	delete m_defaultSampler;

	m_swapChain			= nullptr;
	m_currentShader		= nullptr;
	m_immediateVBO		= nullptr;
	m_frameUBO			= nullptr;
	m_defaultSampler	= nullptr;
	m_defaultShader		= nullptr;

	CleanTextures();

	for( ShaderMapIterator it = m_shaders.begin(); it != m_shaders.end(); ++it ) {
		delete it->second;
	}
	m_shaders.clear();

	DX_SAFE_RELEASE(m_device);
	DX_SAFE_RELEASE(m_context);
	DX_SAFE_RELEASE(m_alphaBlendState);
	DX_SAFE_RELEASE(m_additiveBlendState);
	DX_SAFE_RELEASE(m_opaqueBlendState);
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
	Texture* output = camera.GetColorTarget(); // get texture output

	if( output == nullptr ) {
		output = m_swapChain->GetBackBuffer();
	}

	TextureView* view = output->GetRenderTargetView(); // texture view: handler of the data in texture
	ID3D11RenderTargetView* rtv = view->GetRTVHandle();

	IntVec2 outputSize = output->GetTexelSize();
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX	= 0;
	viewport.TopLeftY	= 0;
	viewport.Width		= (float)output->m_texelSizeCoords.x; //texture->getWidth();		
	viewport.Height		= (float)output->m_texelSizeCoords.y; //texture->GetHeight();		
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

	BindUniformBuffer( UBO_FRAME_SLOT, m_frameUBO );
	
	BindUniformBuffer( UBO_CAMERA_SLOT, cameraUBO );

	Texture* temp = CreateOrGetTextureFromFile( "Data/Fonts/SquirrelFixedFont.png" );
	BindTexture( temp );
	BindSampler( nullptr );

	SetBlendMode( BlendMode::ALPHA );
}

void RenderContext::SetOrthoView( const AABB2& box )
{
	UNUSED( box ); 
}

void RenderContext::EndCamera( const Camera& camera )
{
	UNUSED(camera);
}

void RenderContext::BindTexture( Texture* texture )
{
	Texture* tempTexture = const_cast<Texture*>(texture);
	if( tempTexture == nullptr ) {
		tempTexture = m_texDefaultColor;
	}
	TextureView* shaderResourceView = tempTexture->GetOrCreateShaderResourceView();
	ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetSRVHandle();
	m_context->PSSetShaderResources( 0, 1, &srvHandle ); // texture shader resource
}

void RenderContext::BindSampler( const Sampler* sampler )
{
	Sampler* tempSampler = const_cast<Sampler*>(sampler);
	if( tempSampler == nullptr ) {
		tempSampler = m_defaultSampler;
	}
	ID3D11SamplerState* samplerHandle = tempSampler->GetHandle();
	m_context->PSSetSamplers( 0, 1, &samplerHandle );
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
	BindShader( shader );
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
 
 	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
 	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
 	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
 	int numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
 
 	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
 	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT need for opengl
 	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );
 
 	//int length = sizeof(*imageData)/sizeof(unsigned int);
 
 	// Check if the load was successful
 	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ));
 	GUARANTEE_OR_DIE( numComponents == 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );

	D3D11_TEXTURE2D_DESC desc;
	desc.Width				= imageTexelSizeX;
	desc.Height				= imageTexelSizeY;
	desc.MipLevels			= 1;
	desc.ArraySize			= 1;
	desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage				= D3D11_USAGE_IMMUTABLE; // mip-chains, GPU/DEFAUTE
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags		= 0;
	desc.MiscFlags			= 0;

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = imageData;
	initialData.SysMemPitch = imageTexelSizeX * 4;
	initialData.SysMemSlicePitch = 0; // for 3d texture
	ID3D11Texture2D* texHandle = nullptr;
	// DirectX Creation
	m_device->CreateTexture2D( &desc, &initialData, &texHandle );
	
	
	
	
			
 	Texture* temTexture = new Texture( imageFilePath, this, texHandle );
 	m_textureList.push_back( temTexture );					
 	stbi_image_free( imageData );
 	return temTexture;
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

Texture* RenderContext::CreateTextureFromColor( Rgba8 color )
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width				= 1;
	desc.Height				= 1;
	desc.MipLevels			= 1;
	desc.ArraySize			= 1;
	desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage				= D3D11_USAGE_IMMUTABLE; // mip-chains, GPU/DEFAUTE
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags		= 0;
	desc.MiscFlags			= 0;

	D3D11_SUBRESOURCE_DATA initialData;
	unsigned char imageData[4] = { color.r, color.g, color.b, color.a };
	initialData.pSysMem = imageData;
	initialData.SysMemPitch = 4;
	initialData.SysMemSlicePitch = 0; // for 3d texture

	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc, &initialData, &texHandle );
	Texture* tempTexture = new Texture( this, texHandle );
	m_textureList.push_back( tempTexture );
	return tempTexture;

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
	if( it != m_shaders.end() ) {
		return it->second;
	}
	Shader* shader = new Shader( this );
	shader->CreateFromFile( fileName );
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

void RenderContext::CreateBlendState()
{
	D3D11_BLEND_DESC alphaDesc;
	alphaDesc.AlphaToCoverageEnable = false;
	alphaDesc.IndependentBlendEnable = false;
	alphaDesc.RenderTarget[0].BlendEnable = true;
	alphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; 
		//D3D11_BLEND_SRC_ALPHA;
	alphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	alphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	alphaDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_device->CreateBlendState( &alphaDesc, &m_alphaBlendState );

	// additive
	D3D11_BLEND_DESC additiveDesc;
	additiveDesc.AlphaToCoverageEnable = false;
	additiveDesc.IndependentBlendEnable = false;

	additiveDesc.RenderTarget[0].BlendEnable = true;
	additiveDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	additiveDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	additiveDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_device->CreateBlendState( &additiveDesc, &m_additiveBlendState );

	//
	D3D11_BLEND_DESC opaqueDesc;
	opaqueDesc.AlphaToCoverageEnable = false;
	opaqueDesc.IndependentBlendEnable = false;
	opaqueDesc.RenderTarget[0].BlendEnable = true;
	opaqueDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	opaqueDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	opaqueDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;


	opaqueDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	opaqueDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	opaqueDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	opaqueDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_device->CreateBlendState( &opaqueDesc, &m_opaqueBlendState );

}

BitmapFont* RenderContext::CheckBitmapFontExist( const char* imageFilePath ) const
{
	auto it =  m_loadedFonts.find(imageFilePath);
	if(it != m_loadedFonts.end() ){
		return it->second;
	}
	return nullptr;
}

void RenderContext::CleanTextures()
{
	for( int texIndex = 0; texIndex < m_textureList.size(); texIndex++ ) {
		if( m_textureList[texIndex] != nullptr ) {
			delete m_textureList[texIndex];
			m_textureList[texIndex] = nullptr;
		}
	}
}

Texture* RenderContext::CheckTextureExist(const char* imageFilePath) const
{
	for( int textureIndex = 0; textureIndex < m_textureList.size(); textureIndex++ ){
		if( imageFilePath == m_textureList[textureIndex]->m_imageFilePath ){
			return m_textureList[textureIndex];
		}
	}
	return nullptr;
}

void RenderContext::DrawVertexVector(  std::vector<Vertex_PCU>& vertices )
{
	size_t elementSize = sizeof(Vertex_PCU);
	size_t bufferByteSize = vertices.size() * elementSize;
	m_immediateVBO->Update( &vertices[0], bufferByteSize, elementSize );

	// bind
	BindVertexInput( m_immediateVBO );

	// draw
	Draw( (int)vertices.size(), 0 );
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
 	Vertex_PCU temAABB2[6] = {
 		// triangle2
 		Vertex_PCU(Vec3(bounds.mins,0.f),tint,Vec2(0,0)),
 		Vertex_PCU(Vec3(bounds.maxs.x,bounds.mins.y,0.f),tint,Vec2(1,0)),
 		Vertex_PCU(Vec3(bounds.maxs,0.f),tint,Vec2(1,1)),
 		// triangle2
 		Vertex_PCU(Vec3(bounds.mins,0.f),tint,Vec2(0,0)),
 		Vertex_PCU(Vec3(bounds.mins.x,bounds.maxs.y,0.f),tint,Vec2(0,1)),
 		Vertex_PCU(Vec3(bounds.maxs,0.f),tint,Vec2(1,1)),
 	};
 	DrawVertexArray(6,temAABB2);
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
	float const zeroes[4] = { 0.f, 0.f, 0.f, 0.f };

	ID3D11BlendState* blendStateHandle = nullptr;
	
	switch( blendMode )
	{
	case ALPHA:
		blendStateHandle = m_alphaBlendState;
		break;
	case ADDITIVE:
		blendStateHandle = m_additiveBlendState;
		break;
	case OPAQUE:
		blendStateHandle = m_opaqueBlendState;
		break;
	default:
		break;
	}

	m_context->OMSetBlendState( blendStateHandle, zeroes, (uint)~0 );
}

