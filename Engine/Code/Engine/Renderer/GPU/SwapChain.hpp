#pragma once

class RenderContext;
struct IDXGISwapChain;
class Texture;


class SwapChain {
public:
	explicit SwapChain( RenderContext* owner, IDXGISwapChain* handle );
	~SwapChain();

	void Present( int vsync = 0 );
	Texture* GetBackBuffer();

	// Accessor
	//Texture* GetColorTarget();

public:
	RenderContext* m_owner		= nullptr;
	IDXGISwapChain* m_handle	= nullptr;

private:
	Texture* m_backBuffer	= nullptr; // is color target
};