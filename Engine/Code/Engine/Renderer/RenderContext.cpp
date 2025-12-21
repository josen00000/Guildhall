#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/core/Time/Clock.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Cylinder3.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Engine/Math/ConvexPoly2.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Sampler.hpp" 
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/Material.hpp"
//third party library
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
	// Create resource( swapchain, sampler, shader ...)
	// new function more readbility

	// define swapchain
	
	m_swapChain = CreateSwapChain( window, flags );
	m_defaultShader = new Shader( this );
	m_defaultShader->CreateFromFile( "data/Shader/default.hlsl" );
	
	m_immediateVBO = new VertexBuffer( this, MEMORY_HINT_DYNAMIC );
	m_immediateVBO->SetLayout( Vertex_PCU::s_layout );

	// create uniform buffer
	m_frameUBO			= new RenderBuffer( "frameUBO", this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_modelUBO			= new RenderBuffer( "test1", this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_tintUBO			= new RenderBuffer( "test2", this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_sceneDataUBO		= new RenderBuffer( "test3", this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_dissolveUBO		= new RenderBuffer( "test4", this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );

	m_defaultSampler = new Sampler( this, SAMPLER_BILINEAR );

	m_texDefaultColor = CreateTextureFromColor( Rgba8::WHITE );
	m_texDefaultNormal = CreateTextureFromVec4( Vec4( 0.5f, 0.5f, 1.f, 1.f ) );
	AddTexture( m_texDefaultColor );
	AddTexture(m_texDefaultNormal );

	CreateBlendState();
	CreateDefaultRasterStateDesc();

	InitialLights();
	m_clock = new Clock();

	m_effectCamera = new Camera();
	m_effectCamera->EnableClearColor( Rgba8::BLACK );

}

void RenderContext::Shutdown()
{

	// add debug module if needed
	delete m_swapChain;
	delete m_defaultSampler;
	delete m_defaultShader;
	delete m_immediateVBO;
	delete m_frameUBO;
	delete m_modelUBO;
	delete m_tintUBO;
	delete m_sceneDataUBO;
	SELF_SAFE_RELEASE(m_dissolveUBO);
	SELF_SAFE_RELEASE(m_effectCamera);

	m_swapChain			= nullptr;
	m_currentShader		= nullptr;
	m_immediateVBO		= nullptr;
	m_frameUBO			= nullptr;
	m_modelUBO			= nullptr;
	m_tintUBO			= nullptr;
	m_defaultSampler	= nullptr;
	m_defaultShader		= nullptr;
	m_lastBoundIBO		= nullptr;
	m_lastBoundVBO		= nullptr;

	CleanTextures();
	CleanShaders();
	
	DX_SAFE_RELEASE(m_device);
	DX_SAFE_RELEASE(m_context);
	DX_SAFE_RELEASE(m_alphaBlendState);
	DX_SAFE_RELEASE(m_additiveBlendState);
	DX_SAFE_RELEASE(m_opaqueBlendState);
	DX_SAFE_RELEASE(m_currentDepthStencilState);
	DX_SAFE_RELEASE(m_rasterState);
	DX_SAFE_RELEASE(m_previousLayout);
	//DX_SAFE_RELEASE(m_currentLayout);
	SELF_SAFE_RELEASE(m_defaultRasterStateDesc);
	ReportLiveObjects();
}

void RenderContext::BeginFrame()
{
	double deltaTime = (float)m_clock->GetLastDeltaSeconds();
	UpdateFrameTime( (float)deltaTime );
}

void RenderContext::EndFrame()
{
	Prensent();
}

void RenderContext::BeginCamera( Camera* camera, Convention convention/*=X_RIGHT_Y_UP_Z_BACKWARD*/ )
{
#if defined(RENDER_DEBUG)
	ClearState();
	
#endif

	m_currentCamera = camera;
	// Setup the GPU for a draw
	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	SetBlendMode( BlendMode::BLEND_ALPHA ); // opaque is mostly use default blend mode, alpha for 2d

	BeginCameraRTVAndViewport( camera );

	// binding
	BindShader( static_cast<Shader*>(nullptr) );
	BindRasterState();

	// Bind and set uniform buffer
	RenderBuffer* cameraUBO = camera->GetOrCreateCameraBuffer( this, convention );

	SetModelAndSpecular( Mat44(), 1.f, 1.f );
	
	SetTintColor( Rgba8::WHITE );
	
	BindUniformBuffer( UBO_FRAME_SLOT, m_frameUBO );
	BindUniformBuffer( UBO_CAMERA_SLOT, cameraUBO );
	BindUniformBuffer( UBO_MODEL_SLOT, m_modelUBO );
	BindUniformBuffer( UBO_TINT_SLOT, m_tintUBO );
	BindUniformBuffer( UBO_SCENE_DATA_SLOT, m_sceneDataUBO );
// 	RenderBuffer* testubo = new RenderBuffer( "test1", this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
// 	Mat44 test = Mat44();
// 	testubo->Update( &test, sizeof(Mat44), sizeof(Mat44) );
// 	BindUniformBuffer( 6, testubo );
	//BindUniformBuffer( UBO_DISSOLVE_SLOT, m_dissolveUBO );

	SetDiffuseTexture( nullptr ); //default white texture;
	SetNormalTexture( nullptr ); //default white texture;
	BindSampler( nullptr );
}

void RenderContext::BeginCameraViewport( IntVec2 viewPortSize )
{
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX	= 0;
	viewport.TopLeftY	= 0;
	viewport.Width		= (float)viewPortSize.x;	
	viewport.Height		= (float)viewPortSize.y;		
	viewport.MinDepth	= 0.0f;
	viewport.MaxDepth	= 1.0f;

	m_context->RSSetViewports( 1, &viewport );
}

void RenderContext::BeginCameraRTVAndViewport( Camera* camera )
{
 	Texture* output = camera->GetColorTarget(); // get texture output 	
	std::vector<ID3D11RenderTargetView*> rtvs;
	int rtvCount = camera->GetColorTargetCount();

	if( rtvCount == 0 ) {
		rtvCount = 1;
	}
	rtvs.resize( rtvCount );
	
	for( int i = 0; i < rtvCount; i++ ) {
		rtvs[i] = nullptr;
		Texture* colorTarget = camera->GetColorTarget( i );
		if( colorTarget != nullptr ) {
			TextureView* rtv = colorTarget->GetOrCreateRenderTargetView();
			rtvs[i] = rtv->GetRTVHandle();
		}
		else {
			rtvs[i] = nullptr;
		}
	}
	

	if( camera->m_clearState & CLEAR_COLOR_BIT ){
		Rgba8 clearColor = camera->m_clearColor;
		float  clearFloats[4];
		clearFloats[0] = clearColor.r / 255.f;
		clearFloats[1] = clearColor.g / 255.f;
		clearFloats[2] = clearColor.b / 255.f;
		clearFloats[3] = clearColor.a / 255.f;

		for( int i = 0; i < rtvCount; i++ ) {
			if( rtvs[i] != nullptr ) {
				m_context->ClearRenderTargetView( rtvs[i], clearFloats );
			}
		}
	}
	//TextureView* rtv = output->GetOrCreateRenderTargetView(); // texture view: handler of the data in texture
	//ID3D11RenderTargetView* d3d_rtv = rtv->GetRTVHandle();

	IntVec2 outputSize = output->GetSize();
	BeginCameraViewport( outputSize );

	// depth
	if( camera->IsUseDepth() ) {
		ID3D11DepthStencilView* dsv = nullptr;
		Texture* depthBuffer = camera->GetOrCreateDepthStencilTarget( this );
		TextureView* depthStencilView = depthBuffer->GetOrCreateDepthStencilView();
		dsv = depthStencilView->GetDSVHandle();

		if( camera->IsClearDepth() ) {
			m_context->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH, 1.f, 0 );
		}
		m_context->OMSetRenderTargets( (uint)rtvs.size(), rtvs.data(), dsv ); // dsv always exist
	}
	else {
		m_context->OMSetRenderTargets( (uint)rtvs.size(), rtvs.data(), NULL );
	}

	// color
	if( camera->IsClearColor() ) {
		ClearTargetView( output, camera->GetClearColor() );
	}
}

void RenderContext::EndCamera()
{
	// clean up current state tracker
	m_currentCamera = nullptr;
	m_shaderHasChanged = false;  
	DX_SAFE_RELEASE( m_currentDepthStencilState );
	ClearState();
}

void RenderContext::ClearState()
{
	m_context->ClearState();
	m_lastBoundVBO = nullptr;
	m_lastBoundIBO = nullptr;
	m_previousLayout = nullptr;
	m_currentShader = nullptr;
}

void RenderContext::ClearTargetView( Texture* output, const Rgba8& clearColor )
{
	float clearFolats[4];
	clearFolats[0] = (float)clearColor.r / 255.f;
	clearFolats[1] = (float)clearColor.g / 255.f;
	clearFolats[2] = (float)clearColor.b / 255.f;
	clearFolats[3] = (float)clearColor.a / 255.f;

	TextureView* targetView_rtv = output->GetOrCreateRenderTargetView();
	ID3D11RenderTargetView* rtv = targetView_rtv->GetRTVHandle();

	m_context->ClearRenderTargetView( rtv, clearFolats );
}

Texture* RenderContext::CreateRenderTarget( IntVec2 texSize )
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width				= texSize.x;
	desc.Height				= texSize.y;
	desc.MipLevels			= 1;
	desc.ArraySize			= 1;
	desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage				= D3D11_USAGE_DEFAULT; // mip-chains, GPU/DEFAUTE
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags		= 0;
	desc.MiscFlags			= 0;


	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc, nullptr, &texHandle );

	Texture* newTexture = new Texture( this, texHandle );
	return newTexture;
}

Texture* RenderContext::CreateRenderTargetWithSizeAndData( IntVec2 texSize, void* data )
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width				= texSize.x;
	desc.Height				= texSize.y;
	desc.MipLevels			= 1;
	desc.ArraySize			= 1;
	desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage				= D3D11_USAGE_DEFAULT; // mip-chains, GPU/DEFAUTE
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags		= 0;
	desc.MiscFlags			= 0;

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = data;
	initialData.SysMemPitch = texSize.x * 4;
	initialData.SysMemSlicePitch = 0; // for 3d texture

	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc, &initialData, &texHandle );

	Texture* newTexture = new Texture( this, texHandle );
	return newTexture;
}

void RenderContext::CopyTexture( Texture* dst, Texture* src )
{
	m_context->CopyResource( dst->GetHandle(), src->GetHandle() ); // format and size has to match
}

void RenderContext::StartEffect( Texture* dst, Texture* src, Shader* shader, RenderBuffer* ubo )
{
	m_effectCamera->SetColorTarget( dst );
	//m_effectCamera->DisableClearColor();
	BeginCamera( m_effectCamera, g_convention );
	BindShader( shader );
	SetMaterialBuffer( ubo );
	SetDiffuseTexture( src );
}

void RenderContext::StartBloomEffect( Texture* dst, Texture* src, Texture* color, Shader* shader, RenderBuffer* ubo )
{
	m_effectCamera->SetColorTarget( dst );
	//m_effectCamera->DisableClearColor();
	BeginCamera( m_effectCamera, g_convention );
	BindShader( shader );
	SetMaterialBuffer( ubo );
	SetDiffuseTexture( src );
	SetDiffuseTexture( color, 1 );
}

void RenderContext::EndEffect()
{
	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_context->Draw( 3, 0 );
	EndCamera( );
}

Texture* RenderContext::AcquireRenderTargetMatching( Texture* texture )
{
	IntVec2 size = texture->GetSize();

	for( int i = 0; i < m_renderTargetPool.size(); i++ ) {
		Texture* rt = m_renderTargetPool[i];
		if( rt->GetSize() == size ) {
			m_renderTargetPool[i] = m_renderTargetPool.back();
			m_renderTargetPool.pop_back();
			return rt;
		}
	}

	Texture* tex = CreateRenderTarget( size );
	m_totalRenderTargetMade++;
	return tex;
}

void RenderContext::ReleaseRenderTarget( Texture* tex )
{
	if( tex == nullptr ){ return; }
	m_renderTargetPool.push_back( tex );
}

// 
// void RenderContext::BindTexture( Texture* texture )
// {
// 	Texture* tempTexture = const_cast<Texture*>(texture);
// 	if( tempTexture == nullptr ) {
// 		tempTexture = m_texDefaultColor;
// 	}
// 	TextureView* shaderResourceView = tempTexture->GetOrCreateShaderResourceView();
// 	ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetSRVHandle();
// 	m_context->PSSetShaderResources( 0, 1, &srvHandle ); // texture shader resource
// 														 // can bind in vertex shader 
// 														 // color lUT
// 														 // faster find colro in pixel shader
// 														 // fancy staff
// }

void RenderContext::BindTexture( Texture* texture, uint slot )
{
	Texture* tempTexture = const_cast<Texture*>(texture);
	if( tempTexture == nullptr ) {
		tempTexture = m_texDefaultColor;
	}
	TextureView* shaderResourceView = tempTexture->GetOrCreateShaderResourceView();
	ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetSRVHandle();
	m_context->PSSetShaderResources( slot, 1, &srvHandle ); // texture shader resource
														 // can bind in vertex shader 
														 // color lUT
														 // faster find colro in pixel shader
														 // fancy staff
}

void RenderContext::SetAttenuation( int lightIndex, Vec3 atten, AttenuationType type )
{
	light_t& light = m_sceneData.lights[lightIndex];
	switch( type )
	{
	case ATTENUATION_DIFFUSE:
		light.diffuseAttenuation = atten;
		m_sceneHasChanged = true;
		break;
	case ATTENUATION_SPECULAR:
		light.specularAttenuation = atten;
		m_sceneHasChanged = true;
		break;
	default:
		break;
	}
}

void RenderContext::InitialLights()
{
	for( int i = 0; i < MAX_LIGHTS_NUM; i++ ) {
		DisableLight( i );
	}
}

void RenderContext::CreateDebugModule()
{
	m_debugModule = ::LoadLibraryA( "Dxgidebug.dll" );
	if( m_debugModule == nullptr ) {
		//LogTaggedPrintf( "gfx", "Failed to find dxgidebug.dll.  No debug features enabled." );
	}
	else {
		// find a function in the loaded dll
		typedef HRESULT( WINAPI* GetDebugModuleCB )(REFIID, void**);
		GetDebugModuleCB cb = (GetDebugModuleCB) ::GetProcAddress( m_debugModule, "DXGIGetDebugInterface" );

		// create our debug object
		HRESULT hr = cb( __uuidof(IDXGIDebug), (void**)&m_debug );
		GUARANTEE_OR_DIE( SUCCEEDED( hr ), "what is the error?" );
	}
}

void RenderContext::ReportLiveObjects()
{
	if( nullptr != m_debug ) {
		m_debug->ReportLiveObjects( DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL) );
	}
}

void RenderContext::DestroyDebugModule()
{
	if( nullptr != m_debug ) {
		DX_SAFE_RELEASE( m_debug );   // release our debug object
		FreeLibrary( m_debugModule ); // unload the dll

		m_debug = nullptr;
		m_debugModule = nullptr;
	}
}

void RenderContext::SetDiffuseTexture( Texture* tex, int index )
{
	if( tex == nullptr ) {
		BindTexture( m_texDefaultColor, TEXTURE_DIFFUSE_SLOT0 + index );
	}
	BindTexture( tex, TEXTURE_DIFFUSE_SLOT0 + index );
}

void RenderContext::SetNormalTexture( Texture* texture, int index )
{
	if( texture == nullptr ) {
		BindTexture( m_texDefaultNormal, TEXTURE_NORMAL_SLOT0 + index  );
	}
	else {
		BindTexture( texture, TEXTURE_NORMAL_SLOT0 + index );
	}
}

void RenderContext::SetMaterialTexture( Texture* texture, int index /*= 0 */ )
{
	BindTexture( texture, TEXTURE_USER_SLOT1 + index );
}

void RenderContext::SetFog( float fogFarDist, float fogNearDist, Rgba8 fogFarColor, Rgba8 fogNearColor )
{
	m_sceneData.fog_far_distance = fogFarDist;
	m_sceneData.fog_near_distance = fogNearDist;
	m_sceneData.fog_far_color = fogFarColor.GetVec3Color();
	m_sceneData.fog_near_color = fogNearColor.GetVec3Color();
	m_sceneHasChanged = true;
}

void RenderContext::DisableFog()
{
	m_sceneData.fog_far_distance = 0.1f;
	m_sceneData.fog_near_distance = 0.2f;
	m_sceneHasChanged = true;
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
		shader = m_defaultShader;
	}
	if( m_currentShader == shader ) { return; }

	m_currentShader = shader;

	m_shaderHasChanged = true;
	m_context->VSSetShader( m_currentShader->m_vertexStage.m_vs, nullptr, 0 );
	m_context->PSSetShader( m_currentShader->m_fragmentStage.m_fs, nullptr, 0 );
}

void RenderContext::BindShader( const char* fileName )
{
	Shader* shader = GetOrCreateShader( fileName );
	BindShader( shader );
}

void RenderContext::BindShader( std::string fileName )
{
	BindShader( fileName.c_str() );
}

void RenderContext::BindShaderState( ShaderState* state )
{
	state->PrepareForDraw();
}

void RenderContext::BindRasterState()
{
	if( m_rasterState == nullptr ) {
		m_device->CreateRasterizerState( m_defaultRasterStateDesc, &m_rasterState );
	}
	m_context->RSSetState( m_rasterState ); // use defaults // temp for testing
}

void RenderContext::BindVertexBuffer( VertexBuffer* vbo )
{
	// interlaced format
	// using PCU array of struct
	// if using struct of array
	ID3D11Buffer* vboHandle = vbo->m_handle;
	UINT stride = (UINT)vbo->m_elementByteSize;
	UINT offset = 0; // not do here, for multiple vertexbuffer

	if( m_lastBoundVBO != vboHandle ) {
		m_context->IASetVertexBuffers( 0, 1, &vboHandle, &stride, &offset );
		m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		m_lastBoundVBO = vboHandle;
	}
	m_currentVBO = vbo;
}

void RenderContext::BindIndexBuffer( IndexBuffer* ibo )
{
	ID3D11Buffer* iboHandle = ibo->m_handle;
	UINT offset = 0;

	if( m_lastBoundIBO != iboHandle ) {
		m_context->IASetIndexBuffer( iboHandle, DXGI_FORMAT_R32_UINT, offset );
		m_lastBoundIBO = iboHandle;
	}
}

void RenderContext::BindUniformBuffer( uint slot, RenderBuffer* ubo )
{
	ID3D11Buffer* uboHandle = ubo->m_handle;

	m_context->VSSetConstantBuffers( slot, 1, &uboHandle ); // bind to vertex shader
	m_context->PSSetConstantBuffers( slot, 1, &uboHandle ); // bind to pixel shader
}

void RenderContext::BindMaterial( Material* matl )
{
	BindShaderState( matl->m_shaderState );

	for( int i = 0; i < matl->m_texturePerSlot.size(); i++ ) {
		BindTexture( matl->m_texturePerSlot[i], i );
	}
	BindSampler( matl->m_sampler );
	matl->UpdateUniformBuffer();
	BindUniformBuffer( UBO_MATERIAL_SLOT, matl->m_ubo );
}

void RenderContext::SetBlendMode( BlendMode blendMode )
{
	float const zeroes[4] ={ 0.f, 0.f, 0.f, 0.f };

	ID3D11BlendState* blendStateHandle = nullptr;

	switch( blendMode )
	{
	case BLEND_ALPHA:
		blendStateHandle = m_alphaBlendState;
		break;
	case BLEND_ADDITIVE:
		blendStateHandle = m_additiveBlendState;
		break;
	case BLEND_OPAQUE:
		blendStateHandle = m_opaqueBlendState;
		break;
	default:
		break;
	}
	m_context->OMSetBlendState( blendStateHandle, zeroes, (uint)~0 );
}


void RenderContext::SetModelMatrix( Mat44 model )
{
	m_model.modelMat = model;
	m_modelHasChanged = true;
}

void RenderContext::SetSpecularFactor( float factor )
{
	m_model.specularFactor = factor;
	m_modelHasChanged = true;
}

void RenderContext::SetSpecularPow( float pow )
{
	m_model.specularPow	 = pow;
	m_modelHasChanged = true;
}

void RenderContext::SetModelAndSpecular( Mat44 model, float factor, float pow )
{
	SetModelMatrix( model );
	SetSpecularFactor( factor );
	SetSpecularPow( pow );
}

void RenderContext::SetMaterialBuffer( RenderBuffer* ubo )
{
	BindUniformBuffer( UBO_MATERIAL_SLOT, ubo );
}

void RenderContext::SetOffsetBuffer( RenderBuffer* ubo, int index )
{
	BindUniformBuffer( UBO_OFFSET_SLOT1 + index, ubo );
}

void RenderContext::SetDissolveData( Vec3 startColor, Vec3 endColor, float width, float amount )
{
	m_dissolveData.burnStartColor = startColor;
	m_dissolveData.burnEndColor = endColor;
	m_dissolveData.burnEdgeWidth = width;
	m_dissolveData.burnAmount = amount;
	m_dissolveUBO->Update( &m_dissolveData, sizeof(dissolve_data_t), sizeof(dissolve_data_t) );
}

void RenderContext::SetTintColor( Vec4 color )
{
	tint_color_t tintData;
	tintData.r = color.x;
	tintData.g = color.y;
	tintData.b = color.z;
	tintData.a = color.w;
	m_tintUBO->Update( &tintData, sizeof( tintData ), sizeof( tintData ) );
}

void RenderContext::SetTintColor( Rgba8 color )
{
	Vec4 v_color;
	v_color.x = RangeMapFloat( 0.f, 255.f, 0.f, 1.f, color.r );
	v_color.y = RangeMapFloat( 0.f, 255.f, 0.f, 1.f, color.g );
	v_color.z = RangeMapFloat( 0.f, 255.f, 0.f, 1.f, color.b );
	v_color.w = RangeMapFloat( 0.f, 255.f, 0.f, 1.f, color.a );
	SetTintColor( v_color );
}

void RenderContext::EnableDepth( DepthCompareFunc func, bool writeDepthOnPass )
{
	GUARANTEE_OR_DIE( m_currentCamera != nullptr, std::string( "current camera must not be empty." ) );

	if( m_currentDepthStencilState != nullptr ) {
		if( CheckDepthStencilState( func, writeDepthOnPass ) ) {
			m_context->OMSetDepthStencilState( m_currentDepthStencilState, 1 );
		}
		else {
			DX_SAFE_RELEASE( m_currentDepthStencilState );
		}
	}
	// create depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = D3D11_DEPTH_STENCIL_DESC();
	dsDesc.DepthEnable		= true;
	dsDesc.DepthWriteMask	= writeDepthOnPass ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc		= (D3D11_COMPARISON_FUNC)func;
	m_device->CreateDepthStencilState( &dsDesc, &m_currentDepthStencilState );
	m_context->OMSetDepthStencilState( m_currentDepthStencilState, 1 ); // stencilRef what is this: compare to value
}

void RenderContext::DisableDepth()
{
	// isdrawing();
	GUARANTEE_OR_DIE( m_currentCamera != nullptr, std::string( "current camera must not be empty." ) );

	if( m_currentDepthStencilState != nullptr ) {
		DX_SAFE_RELEASE( m_currentDepthStencilState );
	}
	// create depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = D3D11_DEPTH_STENCIL_DESC();
	dsDesc.DepthEnable		= false;
	m_device->CreateDepthStencilState( &dsDesc, &m_currentDepthStencilState );
	m_context->OMSetDepthStencilState( m_currentDepthStencilState, 1 );
}

void RenderContext::SetDiffuseAttenuation( int lightIndex, Vec3 atten )
{
	SetAttenuation( lightIndex, atten, ATTENUATION_DIFFUSE );
}

void RenderContext::SetSpecularAttenuation( int lightIndex, Vec3 atten )
{
	SetAttenuation( lightIndex, atten, ATTENUATION_SPECULAR );
}

bool RenderContext::CheckDepthStencilState( DepthCompareFunc func, bool writeDepthOnPass )
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	m_currentDepthStencilState->GetDesc( &dsDesc );

	bool writeCheck = dsDesc.DepthWriteMask == D3D11_DEPTH_WRITE_MASK_ALL ? true : false;

	if( writeDepthOnPass != writeCheck ) {
		return false;
	}
	if( dsDesc.DepthFunc != (D3D11_COMPARISON_FUNC)func ) {
		return false;
	}
	return true;
}

void RenderContext::SetCullMode( RasterCullMode mode )
{
	//GUARANTEE_OR_DIE( m_rasterState != nullptr, "Try to set cull mode without having raster state! " );
	if( m_rasterState == nullptr ) {
		m_device->CreateRasterizerState( m_defaultRasterStateDesc, &m_rasterState );
	}

	D3D11_RASTERIZER_DESC desc;
	m_rasterState->GetDesc( &desc );
	ID3D11RasterizerState* state = nullptr;
	
	switch( mode )
	{
		case RASTER_CULL_NONE:
			desc.CullMode = D3D11_CULL_NONE;
			break;
		case RASTER_CULL_FRONT:
			desc.CullMode = D3D11_CULL_FRONT;
			break;
		case RASTER_CULL_BACK:
			desc.CullMode = D3D11_CULL_BACK;
			break;
	}
	m_device->CreateRasterizerState( &desc, &state );

	DX_SAFE_RELEASE( m_rasterState );
	m_rasterState = state;
	BindRasterState();
}

void RenderContext::SetFillMode( RasterFillMode mode )
{
	if( m_rasterState == nullptr ) {
		m_device->CreateRasterizerState( m_defaultRasterStateDesc, &m_rasterState );
	}

	D3D11_RASTERIZER_DESC desc;
	ID3D11RasterizerState* state = nullptr;
	m_rasterState->GetDesc( &desc );


	switch( mode )
	{
		case RASTER_FILL_WIREFRAME:
			desc.FillMode = D3D11_FILL_WIREFRAME;
			break;
		case RASTER_FILL_SOLID:
			desc.FillMode = D3D11_FILL_SOLID;
			break;
	}
	m_device->CreateRasterizerState( &desc, &state );

	DX_SAFE_RELEASE( m_rasterState );
	m_rasterState = state;
	BindRasterState();
}

void RenderContext::SetFrontFaceWindOrder( RasterWindOrder order )
{
	if( m_rasterState == nullptr ) {
		m_device->CreateRasterizerState( m_defaultRasterStateDesc, &m_rasterState );
	}

	D3D11_RASTERIZER_DESC* desc = nullptr;
	ID3D11RasterizerState* state = nullptr;
	m_rasterState->GetDesc( desc );

	switch( order )
	{
	case FRONT_CLOCKWISE:
		desc->FrontCounterClockwise = false;
		break;
	case FRONT_COUNTER_CLOCKWISE:
		desc->FrontCounterClockwise = true;
		break;
	}
	m_device->CreateRasterizerState( desc, &state );

	DX_SAFE_RELEASE( m_rasterState );
	m_rasterState = state;
	BindRasterState();
}

void RenderContext::Draw( int numVertexes, int vertexOffset /*= 0 */ )
{
	UpdateLayoutIfNeeded();
	UpdateSceneIfNeeded();
	UpdateModelIfNeeded();
	m_context->Draw( numVertexes, vertexOffset ); // what is vertexOffset
}

void RenderContext::DrawIndexed( int indexCount, int indexOffset /*= 0*/, int vertexOffset /*= 0 */ )
{
	UpdateLayoutIfNeeded();
	UpdateSceneIfNeeded();
	UpdateModelIfNeeded();
	m_context->DrawIndexed( (uint)indexCount,indexOffset, vertexOffset );
}

void RenderContext::DrawMesh( GPUMesh* mesh )
{
	mesh->UpdateVerticeBuffer( );
	BindVertexBuffer( mesh->GetOrCreateVertexBuffer() );

	bool hasIndices = mesh->GetIndexCount() > 0;
	if( hasIndices ){
		mesh->UpdateIndiceBuffer();
		BindIndexBuffer( mesh->GetOrCreateIndexBuffer() );
		DrawIndexed( mesh->GetIndexCount() );
	}
	else{
		Draw( mesh->GetVertexCount() );
	}
}

void RenderContext::DrawVertexVector( const std::vector<Vertex_PCU>& vertices )
{
	if( vertices.size() == 0 ) {
		return;
	}
	size_t elementSize = sizeof( Vertex_PCU );
	size_t bufferByteSize = vertices.size() * elementSize;
	m_immediateVBO->Update( &vertices[0], bufferByteSize, elementSize );

	// bind
	BindVertexBuffer( m_immediateVBO );

	// draw
	Draw( (int)vertices.size(), 0 );
}

void RenderContext::DrawIndexedVertexVector( const std::vector<Vertex_PCU>& vertices, const std::vector<uint>& indexes )
{
	if( vertices.size() == 0 ) { return;}
	size_t elementSize = sizeof( Vertex_PCU );
	size_t bufferByteSize = vertices.size() * elementSize;
	m_immediateVBO->Update( &vertices[0], bufferByteSize, elementSize );

	// bind
	BindVertexBuffer( m_immediateVBO );

	IndexBuffer indexBuf = IndexBuffer( "test", this, MEMORY_HINT_DYNAMIC );
	indexBuf.Update( indexes );
	BindIndexBuffer( &indexBuf );
	
	DrawIndexed( (int)indexes.size() );
}

void RenderContext::DrawVertexArray( int vertexNum, Vertex_PCU* vertexes )
{
	// Update a vertex buffer
	size_t elementSize = sizeof( Vertex_PCU );
	size_t bufferByteSize = vertexNum * sizeof( Vertex_PCU );
	m_immediateVBO->Update( vertexes, bufferByteSize, elementSize );

	// Bind
	BindVertexBuffer( m_immediateVBO );

	// Draw
	Draw( vertexNum, 0 );
}

void RenderContext::DrawPlane2D( Plane2 const& plane, float drawLength, Rgba8 tint )
{
	Vec2 point = plane.GetDistance() * plane.GetNormal();
	Vec2 tangent = plane.GetNormal();
	tangent.Rotate90Degrees();
	LineSegment2 drawLine( ( point + tangent * drawLength), ( point - tangent * drawLength ));
	DrawLine( drawLine, 0.5f, tint );
	DrawLine( point, point + plane.GetNormal(), 0.5f, tint );
}

void RenderContext::DrawConvexHull( ConvexHull2 const& hull, Rgba8 tint )
{
	for( Plane2 plane : hull.m_planes ) {
		DrawPlane2D( plane, 200.f, tint );
	}
}

void RenderContext::DrawAABB2D( const AABB2& bounds, const Rgba8& tint, const Vec2& uvMin /*= Vec2::ZERO*/, const Vec2& uvMax /*= Vec2::ONE */ )
{
	
	float temZ = 0.f; // set to zero or use default
	Vertex_PCU temAABB2[6] ={
		// triangle1
		Vertex_PCU( Vec3( bounds.mins, temZ ), tint, uvMin ),
		Vertex_PCU( Vec3( bounds.maxs.x, bounds.mins.y, temZ ), tint, Vec2( uvMax.x, uvMin.y ) ),
		Vertex_PCU( Vec3( bounds.maxs, temZ ), tint, uvMax ),
		// triangle2
		Vertex_PCU( Vec3( bounds.mins, temZ ), tint,uvMin ),
		Vertex_PCU( Vec3( bounds.maxs, temZ ), tint, uvMax ),
		Vertex_PCU( Vec3( bounds.mins.x,bounds.maxs.y, temZ ), tint, Vec2( uvMin.x, uvMax.y ) ),
	};
	DrawVertexArray( 6, temAABB2 );
}

void RenderContext::DrawAABB2DWithBound( const AABB2& bounds, const Rgba8& tint, float boundThick, const Rgba8& boundColor )
{
	DrawAABB2D( bounds, tint );
	DrawLine( bounds.mins, Vec2( bounds.maxs.x, bounds.mins.y), boundThick, boundColor );
	DrawLine( Vec2( bounds.maxs.x, bounds.mins.y), bounds.maxs, boundThick, boundColor );
	DrawLine( bounds.maxs, Vec2( bounds.mins.x, bounds.maxs.y), boundThick, boundColor );
	DrawLine( Vec2( bounds.mins.x, bounds.maxs.y), bounds.mins, boundThick, boundColor );
}

void RenderContext::DrawPolygon2D( ConvexPoly2 polygon, Rgba8 tint )
{
	Vec2 centerPos = polygon.GetCenter();
	std::vector<Vertex_PCU> tempVertices;
	std::vector<Vec2> points;
	polygon.GetPoints( points );
	for( int i = 0; i < points.size(); i++ ) {
		Vec2 endPos = ( i == points.size() - 1 ? points[0] : points[i + 1] );
		tempVertices.push_back( Vertex_PCU( Vec3( centerPos ), tint, Vec2::ZERO ) );
		tempVertices.push_back( Vertex_PCU( Vec3( points[i] ), tint, Vec2::ZERO ) );
		tempVertices.push_back( Vertex_PCU( Vec3( endPos ), tint, Vec2::ZERO ) );
	}
	DrawVertexVector( tempVertices );
}

void RenderContext::DrawPolygon2DWithBound( ConvexPoly2 polygon, Rgba8 tint, float boundThick, Rgba8& boundColor )
{
	DrawPolygon2D( polygon, tint );
	
	std::vector<Vec2> points;
	polygon.GetPoints( points );

 	for( int i = 0; i < points.size(); i++ ) {
		int endIndex = ( i == points.size() - 1 ? 0 : i + 1 );
		DrawLine( points[i], points[endIndex], boundThick, boundColor );
	}
}

void RenderContext::DrawConvexPoly2D( ConvexPoly2 convPoly, Rgba8 tint )
{
	std::vector<Vec2> convPolyPoints;
	convPoly.GetPoints( convPolyPoints );
	Vec2 ConvPolyCenter = convPoly.GetCenter();
	std::vector<Vertex_PCU> tempVertices;
	for( int i = 0; i < convPolyPoints.size() - 1; i++ ) {
		tempVertices.push_back( Vertex_PCU( Vec3( ConvPolyCenter ), tint, Vec2::ZERO ) );
		tempVertices.push_back( Vertex_PCU( Vec3( convPolyPoints[i] ), tint, Vec2::ZERO ) );
		tempVertices.push_back( Vertex_PCU( Vec3( convPolyPoints[i+1] ), tint, Vec2::ZERO ) );
	}
	tempVertices.push_back( Vertex_PCU( Vec3( ConvPolyCenter ), tint, Vec2::ZERO ) );
	tempVertices.push_back( Vertex_PCU( Vec3( convPolyPoints.back() ), tint, Vec2::ZERO ) );
	tempVertices.push_back( Vertex_PCU( Vec3( convPolyPoints.front() ), tint, Vec2::ZERO ) );

	for( int i = 0; i < convPolyPoints.size() - 1; i++ ) {
		DrawLine( convPolyPoints[i], convPolyPoints[i+1], 0.2f, Rgba8::RED );
	}
	DrawLine( convPolyPoints.back(), convPolyPoints.front(), 0.2f, Rgba8::RED );

	DrawVertexVector( tempVertices );


}

void RenderContext::DrawLine( const Vec2& startPoint, const Vec2& endPoint, const float thick, const Rgba8& lineColor )
{
	LineSegment2 tem = LineSegment2( startPoint, endPoint );
	DrawLine( tem, thick, lineColor );
}

void RenderContext::DrawLine( const LineSegment2& lineSeg, float thick, const Rgba8& lineColor )
{
	//BindVertexBuffer( m_immediateVBO );
	float halfThick = thick / 2;
	Vec2 direction = lineSeg.GetDisplacement();
	direction.SetLength( halfThick );
	Vec2 tem_position = lineSeg.GetEndPos() + direction;
	Vec2 rightTop = tem_position + (Vec2( -direction.y, direction.x ));
	Vec2 rightdown = tem_position + (Vec2( direction.y, -direction.x ));
	Vec2 tem_position1 = lineSeg.GetStartPos() - direction;
	Vec2 leftTop = tem_position1 + (Vec2( -direction.y, direction.x ));
	Vec2 leftdown = tem_position1 + (Vec2( direction.y, -direction.x ));
	Vec2 tem_uv = Vec2( 0.f, 0.f );
	Vertex_PCU line[6]={
		Vertex_PCU( Vec3( rightTop.x,rightTop.y,0 ),	Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( rightdown.x,rightdown.y,0 ),	Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( leftTop.x,leftTop.y,0 ),		Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( leftTop.x,leftTop.y, 0 ),		Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( leftdown.x,leftdown.y, 0 ),	Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( rightdown.x,rightdown.y, 0 ), Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) )
	};

	DrawVertexArray( 6, line );
}

void RenderContext::DrawLineWithHeight( const LineSegment2& lineSeg, float height, float thick, const Rgba8& lineColor )
{
	float halfThick = thick / 2;
	Vec2 direction = lineSeg.GetDisplacement();
	direction.SetLength( halfThick );
	Vec2 tem_position = lineSeg.GetEndPos() + direction;
	Vec2 rightTop = tem_position + (Vec2( -direction.y, direction.x ));
	Vec2 rightdown = tem_position + (Vec2( direction.y, -direction.x ));
	Vec2 tem_position1 = lineSeg.GetStartPos() - direction;
	Vec2 leftTop = tem_position1 + (Vec2( -direction.y, direction.x ));
	Vec2 leftdown = tem_position1 + (Vec2( direction.y, -direction.x ));
	Vec2 tem_uv = Vec2( 0.f, 0.f );
	Vertex_PCU line[6]={
		Vertex_PCU( Vec3( rightTop.x,rightTop.y,height ),	Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( rightdown.x,rightdown.y,height ),	Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( leftTop.x,leftTop.y,height ),		Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( leftTop.x,leftTop.y, height ),		Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( leftdown.x,leftdown.y, height ),	Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( rightdown.x,rightdown.y, height ), Rgba8( lineColor.r,lineColor.g,lineColor.b ),	Vec2( 0, 0 ) )
	};

	DrawVertexArray( 6, line );
}

void RenderContext::DrawCircle( Vec3 center, float radius, float thick, const Rgba8& circleColor )
{
	float degree = 0;
	float nextDegree = 0;
	const int vertexNum = 32;
	for( int i = 0; i < vertexNum; i++ ) {
		nextDegree = (i + 1) * (360.f / vertexNum);
		Vec2 startPoint = Vec2();
		startPoint.SetPolarDegrees( degree, radius );
		startPoint += Vec2( center );
		Vec2 endPoint = Vec2();
		endPoint.SetPolarDegrees( nextDegree, radius );
		endPoint += Vec2( center );
		degree = nextDegree;
		DrawLine( startPoint, endPoint, thick, circleColor );
	}
}

void RenderContext::DrawFilledCircle( Vec3 center, float radiu, const Rgba8& filledColor )
{
	float degree = 0;
	float nextDegree = 0;
	const int vertexNum = 32;
	std::vector<Vertex_PCU> circleVertices;
	circleVertices.reserve( vertexNum  );
	for( int i = 0; i < vertexNum; i++ ) {
		nextDegree = (i + 1) * (360.f / vertexNum);
		Vec2 startPoint = Vec2();
		startPoint.SetPolarDegrees( degree, radiu );
		startPoint += (Vec2( center ));
		Vec2 endPoint = Vec2();
		endPoint.SetPolarDegrees( nextDegree, radiu );
		endPoint += (Vec2( center ));
		degree = nextDegree;
		circleVertices.push_back( Vertex_PCU( center, filledColor, Vec2::ZERO ) );
		circleVertices.push_back( Vertex_PCU( Vec3( startPoint ), filledColor, Vec2::ZERO ) );
		circleVertices.push_back( Vertex_PCU( Vec3( endPoint ), filledColor, Vec2::ZERO ) );
	}
	DrawVertexVector( circleVertices );
}

void RenderContext::DrawCylinder( const Cylinder3& cylinder, int level, const Rgba8& tintColor )
{
	std::vector<Vertex_PCU> cylinderVertices;
	cylinderVertices.reserve( 100 );
	AppendVertsForCylinder3D( cylinderVertices, cylinder, level, tintColor, tintColor );
	DrawVertexVector( cylinderVertices );
}

Texture* RenderContext::GetSwapChainBackBuffer()
{
	return m_swapChain->GetBackBuffer();
}

BitmapFont* RenderContext::CreateOrGetBitmapFontFromFile( const char* fontName, const char* fontFilePath )
{
	BitmapFont* temFont = CheckBitmapFontExist( fontName );
	if( temFont != nullptr ) { return temFont; }

	temFont = CreateBitmapFontFromFile( fontName, fontFilePath );
	return temFont;
}

Texture* RenderContext::CreateOrGetTextureFromFile( const char* imageFilePath )
{
	Texture* temTexture = CheckTextureExist( imageFilePath );
	if( temTexture != nullptr ) {
		return temTexture;
	}
	else {
		temTexture = CreateTextureFromFile( imageFilePath );
		m_textureList.push_back( temTexture );
		return temTexture;
	}
}

Texture* RenderContext::CreateTextureFromColor( Rgba8 color )
{
	// get or create
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
	unsigned char imageData[4] ={ color.r, color.g, color.b, color.a };
	initialData.pSysMem = imageData;
	initialData.SysMemPitch = 4;
	initialData.SysMemSlicePitch = 0; // for 3d texture

	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc, &initialData, &texHandle );
	Texture* tempTexture = new Texture( this, texHandle );
	return tempTexture;
}

Texture* RenderContext::CreateTextureFromVec4( Vec4 input )
{
	Rgba8 inputColor = Rgba8::GetColorFromVec4( input );
	Texture* tempTex = CreateTextureFromColor( inputColor );
	//m_textureList.push_back( tempTex );
	return tempTex;
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
	m_shaders.insert( std::pair<std::string, Shader*>( tempName, shader ) );
	return shader;
}

SwapChain* RenderContext::CreateSwapChain( Window* window, uint flags )
{
	IDXGISwapChain* swapchain;
	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	memset( &swapchainDesc, 0, sizeof( swapchainDesc ) );
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;

	// how many back buffers in our chain - we'll double buffer (one we show, one we draw to)
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // on swap, the old buffer is discarded
	swapchainDesc.Flags = 0; // additional flags - see docs.  Used in special cases like for video buffers
							 //swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;

	HWND hwnd = (HWND)window->m_hwnd;
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
	return new SwapChain( this, swapchain );
}

void RenderContext::AddTexture( Texture* tex )
{
	m_textureList.push_back( tex );
}


// private
void RenderContext::UpdateLayoutIfNeeded()
{
	 //happen in both draw
	if( m_previousLayout != m_currentLayout || m_shaderHasChanged ){
		m_currentLayout = m_currentShader->GetOrCreateInputLayout( m_currentVBO );// need change later
		m_context->IASetInputLayout( m_currentLayout );
		//DX_SAFE_RELEASE(m_previousLayout);
		m_previousLayout = m_currentLayout;
		m_shaderHasChanged = false;
	}
	else{
		m_context->IASetInputLayout( m_currentLayout );
	}
}

void RenderContext::UpdateSceneIfNeeded()
{
	if( m_sceneHasChanged ) {
		m_sceneDataUBO->Update( &m_sceneData, sizeof( m_sceneData ), sizeof( m_sceneData ) );
		m_sceneHasChanged = false;
	}
}

void RenderContext::UpdateModelIfNeeded()
{
	if( m_modelHasChanged ) {
		m_modelUBO->Update( &m_model, sizeof(m_model), sizeof(m_model) );
		m_modelHasChanged = false;
	}
}

void RenderContext::UpdateFrameTime( float deltaSeconds )
{
	time_data_t frameData;
	frameData.system_time = (float)GetCurrentTimeSeconds();
	frameData.system_delta_time = deltaSeconds;

	m_frameUBO->Update( &frameData, sizeof( frameData ), sizeof( frameData ) );
}

void RenderContext::CreateBlendState()
{
	D3D11_BLEND_DESC alphaDesc;
	alphaDesc.AlphaToCoverageEnable = false;
	alphaDesc.IndependentBlendEnable = false;
	alphaDesc.RenderTarget[0].BlendEnable = true;
	alphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	alphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	alphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
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
	additiveDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_device->CreateBlendState( &additiveDesc, &m_additiveBlendState );

	// Opaque
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

void RenderContext::createRasterState()
{
	D3D11_RASTERIZER_DESC desc;
	desc.FillMode				= D3D11_FILL_SOLID;
	desc.CullMode				= D3D11_CULL_NONE;
	desc.FrontCounterClockwise	= TRUE;
	desc.DepthBias				= 0U;
	desc.DepthBiasClamp			= 0.0f;
	desc.SlopeScaledDepthBias	= 0.0f;
	desc.DepthClipEnable		= TRUE;
	desc.ScissorEnable			= FALSE;
	desc.MultisampleEnable		= FALSE;
	desc.AntialiasedLineEnable	= FALSE;

	m_device->CreateRasterizerState( &desc, &m_rasterState );
}

void RenderContext::SetAmbientColor( Rgba8 color )
{
	Vec3 v3_color = color.GetVec3Color();
	m_sceneData.ambient.SetXYZ( v3_color );
	m_sceneHasChanged = true;
}

void RenderContext::SetAmbientIntensity( float intensity )
{
	m_sceneData.ambient.SetW( intensity );
	m_sceneHasChanged = true;
}

void RenderContext::SetAmbientLight( Rgba8 color, float intensity )
{
	SetAmbientColor( color );
	SetAmbientIntensity( intensity );
}

void RenderContext::DisableAmbient()
{
	SetAmbientLight( Rgba8::WHITE, 1.f );
}

void RenderContext::EnableLight( int index, const light_t& lightInfo )
{
	m_sceneData.lights[index] = lightInfo;
	m_sceneHasChanged = true;
}

void RenderContext::EnablePointLight( int index, Vec3 position, Rgba8 color, float intensity, Vec3 attenuation )
{
	light_t& light = m_sceneData.lights[index];
	light.position = position;
	light.color.SetXYZ( color.GetVec3Color() );
	light.color.SetW (intensity );
	light.diffuseAttenuation = attenuation;
	light.specularAttenuation = attenuation;
	light.direction_factor = 0.f;
	m_sceneHasChanged = true;
}

void RenderContext::EnableDirectionLight( int index, Vec3 position, Rgba8 color, float intensity, Vec3 attenuation )
{
	light_t& light = m_sceneData.lights[index];
	light.position = position;
	light.color.SetXYZ( color.GetVec3Color() );
	light.color.SetW( intensity );
	light.diffuseAttenuation = attenuation;
	light.specularAttenuation = attenuation;
	light.direction_factor = 1.f;
	m_sceneHasChanged = true;
}

void RenderContext::EnableSpotLight( int index, Vec3 position, Rgba8 color, float intensity, Vec3 attenuation, Vec3 direction, float halfInnerDegrees, float halfOuterDegrees )
{
	light_t& light = m_sceneData.lights[index];
	light.position = position;
	light.color.SetXYZ( color.GetVec3Color() );
	light.color.SetW( intensity );
	light.diffuseAttenuation = attenuation;
	light.specularAttenuation = attenuation;
	light.direction = direction;
	light.cos_inner_half_angle = CosDegrees( halfInnerDegrees );
	light.cos_outer_half_angle = CosDegrees( halfOuterDegrees );
	m_sceneHasChanged = true;
}

void RenderContext::DisableLight( int index )
{
	m_sceneData.lights[index].color.SetW( 0.f );
	m_sceneHasChanged = true;
}

void RenderContext::Prensent()
{
	m_swapChain->Present();
}

BitmapFont* RenderContext::CheckBitmapFontExist( const char* imageFilePath ) const
{
	auto it =  m_loadedFonts.find( imageFilePath );
	if( it != m_loadedFonts.end() ) {
		return it->second;
	}
	return nullptr;
}

BitmapFont* RenderContext::CreateBitmapFontFromFile( const char* fontName, const char* fontFilePath )
{
	std::string pngFilePath = fontFilePath;
	pngFilePath.append( ".png" );
	const char* temPath = pngFilePath.c_str();
	Texture* fontTexture = CreateOrGetTextureFromFile( temPath );
	BitmapFont* tem =  new BitmapFont( fontName, fontTexture );
	return tem;
}

Texture* RenderContext::CreateTextureFromFile( const char* imageFilePath )
{
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

									// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT need for opengl
	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );

	//int length = sizeof(*imageData)/sizeof(unsigned int);

	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ) );
	GUARANTEE_OR_DIE(  imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );

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

	//float b= 3;
	//int a = sizeof(b);
	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = imageData;
	initialData.SysMemPitch = imageTexelSizeX * 4;
	initialData.SysMemSlicePitch = 0; // for 3d texture
	ID3D11Texture2D* texHandle = nullptr;
	// DirectX Creation
	m_device->CreateTexture2D( &desc, &initialData, &texHandle );

	Texture* temTexture = new Texture( imageFilePath, this, texHandle );
	stbi_image_free( imageData );
	return temTexture;
}

void RenderContext::CreateCubeMapTexturesFromImages( const Image* src )
{
	int width = src[0].GetDimensions().x;
	int height = src[0].GetDimensions().y;
	D3D11_TEXTURE2D_DESC cubeDesc;
	cubeDesc.Width				= width;
	cubeDesc.Height				= height;
	cubeDesc.MipLevels			= 1;
	cubeDesc.ArraySize			= 6;
	cubeDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	cubeDesc.SampleDesc.Count	= 1;
	cubeDesc.SampleDesc.Quality = 0;
	cubeDesc.Usage				= D3D11_USAGE_DEFAULT; // mip-chains, GPU/DEFAUTE
	cubeDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	cubeDesc.CPUAccessFlags		= 0;
	cubeDesc.MiscFlags			= 0;

	D3D11_SUBRESOURCE_DATA data[TEXCUBE_NUM];
	D3D11_SUBRESOURCE_DATA *pdata = nullptr;

	if( nullptr != src ) {
		pdata = data;
		uint pitch = width * 4; // temp for rgba.

		for( int i = 0; i <TEXCUBE_NUM; i++ ) {
			data[i].pSysMem = src[i].GetImageData().data();
			data[i].SysMemPitch = pitch;
		}

		// create resource
		ID3D11Texture2D* texHandle = nullptr;
		m_device->CreateTexture2D( &cubeDesc, pdata, &texHandle );


	}
}

Texture* RenderContext::CheckTextureExist( const char* imageFilePath ) const
{
	for( int textureIndex = 0; textureIndex < m_textureList.size(); textureIndex++ ) {
		if( imageFilePath == m_textureList[textureIndex]->m_imageFilePath ) {
			return m_textureList[textureIndex];
		}
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

	GUARANTEE_OR_DIE( m_renderTargetPool.size() == m_totalRenderTargetMade, "bug" );
	for( int i = 0; i < m_renderTargetPool.size(); i++ ){
		SELF_SAFE_RELEASE( m_renderTargetPool[i] );
	}
	m_texDefaultColor = nullptr;
	m_texDefaultNormal = nullptr;
}

void RenderContext::CleanShaders()
{
	for( ShaderMapIterator it = m_shaders.begin(); it != m_shaders.end(); ++it ) {
		SELF_SAFE_RELEASE(it->second);
	}
	m_shaders.clear();
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
