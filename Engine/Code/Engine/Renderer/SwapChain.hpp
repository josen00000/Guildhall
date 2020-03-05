#pragma once


class RenderContext;
struct IDXGISwapChain;
class Texture;


class SwapChain {
public:
	SwapChain( RenderContext* owner, IDXGISwapChain* handle);
	~SwapChain();

	void Present( int vsync = 0 );
	Texture* GetBackBuffer();

	// Accessor
	//Texture* GetColorTarget();


private:
	Texture* m_backBuffer	= nullptr; // is color target

public:
	RenderContext* m_owner;
	IDXGISwapChain* m_handle;

};