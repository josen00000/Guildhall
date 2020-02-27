#include "TextureView.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

TextureView::~TextureView()
{
	DX_SAFE_RELEASE(m_handle);
}

void TextureView::SetRTVHandle( ID3D11RenderTargetView* rtv )
{
	m_rtv = rtv;
}

void TextureView::SetSRVHandle( ID3D11ShaderResourceView* srv )
{
	m_srv = srv;
}
