#if !defined(WIN32_LEAN_AND_MEAN)
	#define WIN32_LEAN_AND_MEAN
#endif

#define RENDER_DEBUG
#define DX_SAFE_RELEASE(ptr) if(nullptr !=ptr) { ptr->Release(); ptr = nullptr; } 

#define INITGUID
#include <d3d11.h>
#include <dxgi.h>
#include <dxgidebug.h>