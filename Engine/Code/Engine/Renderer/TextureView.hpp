#pragma once


struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Resource;

class TextureView {
public:
	TextureView(){}
	~TextureView();
	ID3D11RenderTargetView* GetRTVHandle() const { return m_rtv; }
	ID3D11ShaderResourceView* GetSRVHandle() const { return m_srv; }
	void SetRTVHandle( ID3D11RenderTargetView* rtv );
	void SetSRVHandle( ID3D11ShaderResourceView* srv );
public:
	union
	{
		ID3D11Resource* m_handle; // top level for release
		ID3D11RenderTargetView* m_rtv;
		ID3D11ShaderResourceView* m_srv;

	};
};