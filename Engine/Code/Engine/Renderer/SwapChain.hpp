#pragma once


class RenderContext_d3d11;
struct IDXGISwapChain;
class Texture;


class SwapChain {
public:
	SwapChain( RenderContext_d3d11* owner, IDXGISwapChain* handle);
	~SwapChain();

	void Present( int vsync = 0 );
	Texture* GetBackBuffer();

	// Accessor
	//Texture* GetColorTarget();


private:
	Texture* m_backBuffer	= nullptr; // is color target

public:
	RenderContext_d3d11* m_owner;
	IDXGISwapChain* m_handle;

};