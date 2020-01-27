#include "SwapChain.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"


SwapChain::SwapChain( RenderContext* owner, IDXGISwapChain* handle )
	:m_owner(owner)
	,m_handle(handle)
{

}

SwapChain::~SwapChain()
{
	m_owner = nullptr;
	delete m_backBuffer;
	m_backBuffer =nullptr;
	DX_SAFE_RELEASE(m_handle);
}

void SwapChain::Present( int vsync /*= 0 */ )
{
	m_handle->Present( vsync, 0 );
}

Texture* SwapChain::GetBackBuffer() 
{
	if( nullptr != m_backBuffer ) {
		return m_backBuffer;
	}

	ID3D11Texture2D* texHandle = nullptr;
	m_handle->GetBuffer( 0, __uuidof(ID3D11Texture2D),(void**)&texHandle );

	m_backBuffer = new Texture( m_owner, texHandle );
	 
	return m_backBuffer;
}
