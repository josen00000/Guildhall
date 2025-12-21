#pragma once


struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Resource;

class TextureView {
public:
	TextureView(){}
	~TextureView();

public:
	// Accessor
	ID3D11RenderTargetView*		GetRTVHandle() const { return m_rtv; }
	ID3D11ShaderResourceView*	GetSRVHandle() const { return m_srv; }
	ID3D11DepthStencilView*		GetDSVHandle() const { return m_dsv; }

	// Mutator
	void SetRTVHandle( ID3D11RenderTargetView* rtv );
	void SetSRVHandle( ID3D11ShaderResourceView* srv );
	void SetDSVHandle( ID3D11DepthStencilView* dsv );

public:
	union
	{
		ID3D11Resource*				m_handle; // top level for release
		ID3D11RenderTargetView*		m_rtv;
		ID3D11ShaderResourceView*	m_srv;
		ID3D11DepthStencilView*		m_dsv;
	};
};