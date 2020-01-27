#include "TextureView.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

TextureView::~TextureView()
{
	DX_SAFE_RELEASE(m_rtv);
}

void TextureView::SetRTVHandle( ID3D11RenderTargetView* rtv )
{
	m_rtv = rtv;
}
