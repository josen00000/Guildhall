#pragma once


struct ID3D11RenderTargetView;


class TextureView {
public:
	TextureView(){}
	~TextureView();
	ID3D11RenderTargetView* GetRTVHandle() const { return m_rtv; };
	void SetRTVHandle( ID3D11RenderTargetView* rtv );
private:
	ID3D11RenderTargetView* m_rtv;
};