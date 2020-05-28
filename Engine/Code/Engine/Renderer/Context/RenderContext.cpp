#include "RenderContext.hpp"
#include <vector>
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/Context/Camera.hpp"
#include "Engine/Renderer/GPU/Sampler.hpp"
#include "Engine/Renderer/GPU/Shader.hpp"
#include "Engine/Renderer/GPU/SwapChain.hpp"
#include "Engine/Renderer/GPU/Texture.hpp"
#include "Engine/Renderer/GPU/TextureView.hpp"
#include "Engine/Renderer/GPU/VertexBuffer.hpp"

// third party library
#include "Engine/stb_image.h"

#pragma comment( lib, "d3d11.lib" )         // needed a01
#pragma comment( lib, "dxgi.lib" )          // needed a01
#pragma comment( lib, "d3dcompiler.lib" )   // needed when we get to shaders

#define RENDER_DEBUG

void RenderContext::StartUp( Window* window )
{
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(RENDER_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
	CreateDebugModule();
#endif

	// Initial context
	// create resource( swapchain sampler, shader ... )
	// define swapchain
	m_swapChain = CreateSwapChain( window, flags );
	m_defaultShader = new Shader( this );
	m_defaultShader->CreateFromFile( DEFAULT_SHADER_LOCATION );

	m_defaultVBO = new VertexBuffer( this, MEMORY_HINT_DYNAMIC );
	m_defaultVBO->SetLayout( Vertex_PCU::s_layout );

	// create uniform buffer
	m_frameUBO		= new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_modelUBO		= new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_tintUBO		= new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_sceneDataUBO	= new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	//m_frameUBO = new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	//m_frameUBO = new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );

	m_defaultSampler = new Sampler( this, SAMPLER_BILINEAR );

	m_texDefaultColor = CreateTextureFromColor( Rgba8::WHITE );
	m_texDefaultNormal = CreateTextureFromVec4( Vec4( 0.5f, 0.5f, 1.f, 1.f ) );
	AddTexture( m_texDefaultColor );
	AddTexture( m_texDefaultNormal );

	CreateBlendStates();
	CreateDefaultRasterStateDesc();

	InitialLights();

	m_clock = new Clock();

	m_effectCamera = new Camera();
	m_effectCamera->EnableClearColor( Rgba8::BLACK );
}

void RenderContext::Shutdown()
{

}

void RenderContext::BeginFrame()
{
	// update time
	double deltaSeconds = m_clock->GetLastDeltaSeconds();
	UpdateFrameTime( (float)deltaSeconds );
}

void RenderContext::EndFrame()
{
	// present
	m_swapChain->Present();
}

void RenderContext::BeginCamera( Camera* camera )
{
#if defined(RENDER_DEBUG)
	ClearState();
#endif

	m_currentCamera = camera;	
	// // setup gpu for draw
	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	SetBlendMode( m_currentBlendMode );
	SetRenderTarget( m_currentCamera );
	SetModelAndSpecular( Mat44::IDENTITY, 1.f, 1.f );

	// viewport
	Texture* output = camera->GetColorTarget();
	IntVec2 outputSize = output->GetTexelSize();
	SetCameraViewport( outputSize );

	// bind
	BindShader( static_cast<Shader*>(nullptr) );
	BindSampler( nullptr );
}

void RenderContext::ClearTargetView( Texture* output, const Rgba8& clearColor )
{

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
	if( shader == nullptr ) {
		shader = m_defaultShader;
	}

	if( shader == m_currentShader ){ return; }

	m_currentShader = shader;
	m_shaderHasChanged = true;
	m_context->VSSetShader( m_currentShader->m_vertexStage.m_vs, nullptr, 0 );
	m_context->PSSetShader( m_currentShader->m_fragmentStage.m_fs, nullptr, 0 );
}

void RenderContext::SetModelAndSpecular( Mat44 model, float factor, float pow )
{
	SetModelMatrix( model );
	SetSpecularFactor( factor );
	SetSpecularPow( pow );
}

void RenderContext::SetBlendMode( BlendMode blendMode )
{
	float const zeroes[4] = { 0.f, 0.f, 0.f, 0.f };
	switch( blendMode )
	{
	case BLEND_ALPHA:		m_context->OMSetBlendState( m_alphaBlendState, zeroes, (uint)~0  ); 
		break;
	case BLEND_ADDITIVE:	m_context->OMSetBlendState( m_additiveBlendState, zeroes, (uint)~0  ); 
		break;
	case BLEND_OPAQUE:		m_context->OMSetBlendState( m_opaqueBlendState, zeroes, (uint)~0  ); 
		break;
	default:
		break;
	}
	return;
}

void RenderContext::SetModelMatrix( Mat44 model )
{
	m_model.model = 
}

void RenderContext::AddTexture( Texture* tex )
{
	m_textureList.push_back( tex );
}

void RenderContext::CreateBlendStates()
{
	m_alphaBlendState		= CreateBlendState( m_device, BLEND_ALPHA );
	m_additiveBlendState	= CreateBlendState( m_device, BLEND_ADDITIVE );
	m_opaqueBlendState		= CreateBlendState( m_device, BLEND_OPAQUE );
}


ID3D11BlendState* CreateBlendState( ID3D11Device* dev, BlendMode mode )
{
	ID3D11BlendState* blendState = nullptr;
	D3D11_BLEND_DESC desc;
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[0].BlendEnable = true;

	switch( mode )
	{
	case BLEND_ALPHA:
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_ADDITIVE:
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_OPAQUE:
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	default:
		break;
	}

	dev->CreateBlendState( &desc, &blendState );
	return blendState;
}

SwapChain* RenderContext::CreateSwapChain( Window* window, uint flags )
{
	IDXGISwapChain* swapChain;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	memset( &swapChainDesc, 0, sizeof(swapChainDesc) );
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	swapChainDesc.BufferCount = 2;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = 0;

	HWND hwnd = (HWND)window->GetHandler();
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.Width = window->GetClientWidth();
	swapChainDesc.BufferDesc.Height = window->GetClientHeight();

	D3D11CreateDeviceAndSwapChain(	
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain,
		&m_device,
		nullptr,
		&m_context
	);

	return new SwapChain( this, swapChain );
}

void RenderContext::CreateDebugModule()
{
	m_debugModule = ::LoadLibraryA( "Dxgidebug.dll" );
	if( m_debugModule == nullptr ) {
		GUARANTEE_RECOVERABLE( false, "no debug module" );
	}
	else {
		typedef HRESULT( WINAPI* GetDebugModuleCB )(REFIID, void**);
		GetDebugModuleCB cb = (GetDebugModuleCB) ::GetProcAddress( m_debugModule, "DXGIGetDebugInterface" );

		// create our debug object
		HRESULT hr = cb( __uuidof(IDXGIDebug), (void**)&m_debug );
		GUARANTEE_OR_DIE( SUCCEEDED( hr ), "what is the error?" );
	}
}

void RenderContext::SetCameraViewport( IntVec2 viewportSize )
{
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX	= 0;
	viewport.TopLeftY	= 0;
	viewport.Height		= (float)viewportSize.y;
	viewport.Width		= (float)viewportSize.x;
	viewport.MaxDepth	= 1.f;
	viewport.MinDepth	= 0.f;

	m_context->RSSetViewports( 1, &viewport );
}

void RenderContext::SetRenderTarget( Camera* camera )
{
	// render target view
	std::vector<ID3D11RenderTargetView*> rtvs;
	int rtvCount = camera->GetColorTargetCount();
	rtvs.resize( rtvCount );

	for( int i = 0; i < rtvCount; i++ ) {
		rtvs[i] = nullptr;
		Texture* colorTarget = camera->GetColorTarget( i );
		TextureView* rtv = colorTarget->GetOrCreateRenderTargetView();
		rtvs[i] = rtv->GetRTVHandle();

		// clear color
		if( camera->m_clearState & CLEAR_COLOR_BIT && rtvs[i] != nullptr ) {
			Rgba8 clearColor = camera->GetClearColor();
			Vec4 clearColor_v4 = clearColor.GetVec4Color();
			m_context->ClearRenderTargetView( rtvs[i], (const float*)&clearColor_v4 ); // might have error
		}
	}

	// depth stencil view
	ID3D11DepthStencilView* dsv = nullptr;
	if( camera->IsUseDepth() ) {
		Texture* depthBuffer = camera->GetOrCreateDepthStencilTarget( this );
		TextureView* depthStencilView = depthBuffer->GetOrCreateDepthStencilView();
		dsv = depthStencilView->GetDSVHandle();

		if( camera->IsClearDepth() ) {
			m_context->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH, 1.f, 0 );
		}
	}

	m_context->OMSetRenderTargets( (uint)rtvs.size(), rtvs.data(), dsv );
}

void RenderContext::CreateDefaultRasterStateDesc()
{
	m_defaultRasterStateDesc = new D3D11_RASTERIZER_DESC();

	m_defaultRasterStateDesc->FillMode				= D3D11_FILL_SOLID;
	m_defaultRasterStateDesc->CullMode				= D3D11_CULL_NONE;
	m_defaultRasterStateDesc->FrontCounterClockwise	= TRUE;
	m_defaultRasterStateDesc->DepthBias				= 0U;
	m_defaultRasterStateDesc->DepthBiasClamp		= 0.0f;
	m_defaultRasterStateDesc->SlopeScaledDepthBias	= 0.0f;
	m_defaultRasterStateDesc->DepthClipEnable		= TRUE;
	m_defaultRasterStateDesc->ScissorEnable			= FALSE;
	m_defaultRasterStateDesc->MultisampleEnable		= FALSE;
	m_defaultRasterStateDesc->AntialiasedLineEnable	= FALSE;
}

