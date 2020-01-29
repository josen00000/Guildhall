#pragma once
#include <map>
#include <string>
#include "Engine/Core/Vertex_PCU.hpp"


class BitmapFont;
class Camera;
class Texture;
class SwapChain;
class Window;
class Shader;
struct AABB2;
struct ID3D11Device;
struct ID3D11DeviceContext;


enum BlendMode
{
	ALPHA,
	ADDITIVE,
	NUM_BLENDMODE
};


class RenderContext{
public :
	RenderContext(){}
	~RenderContext(){}

public:
	void StartUp( Window* window );
	void Shutdown();
	void BeginView();
	
	void BeginCamera(const Camera& camera);
	void EndCamera(const Camera& camera);
	
	void BeginFrame();
	void EndFrame();
	void ClearScreen(const Rgba8& clearColo);
	void SetOrthoView(const AABB2& box);
	void BindTexture(const Texture* texture);

	void Draw( int numVertexes, int vertexOffset = 0 );
	void DrawVertexVector(const std::vector<Vertex_PCU>& vertexArray);
	void DrawVertexArray(int vertexNum, Vertex_PCU* vertexArray);
	void DrawAABB2D(const AABB2& bounds,const Rgba8& tint);
	void DrawLine(const Vec2& startPoint,const Vec2&endPoint,const float thick,const Rgba8& lineColor);
	void DrawCircle(Vec3 center,float radiu,float thick,Rgba8& circleColor);


	Texture* CreateOrGetTextureFromFile(const char* imageFilePath);
	BitmapFont* CreateOrGetBitmapFontFromFile(const char* fontName, const char* fontFilePath);
	
	void SetBlendMode(BlendMode blendMode);

private:
	Texture*		CreateTextureFromFile(const char* imageFilePath);
	BitmapFont*		CreateBitmapFontFromFile(const char* fontName, const char* fontFilePath);
	Texture*		CheckTextureExist(const char* imageFilePath) const;
	BitmapFont*		CheckBitmapFontExist( const char* fontName ) const;

public:
	ID3D11DeviceContext*	m_context	= nullptr; // how we issue command
	ID3D11Device*			m_device	= nullptr; // reference to gpu
	SwapChain*				m_swapChain	= nullptr;

private:
	std::vector <Texture*> m_textureList;
	std::map<std::string, Texture*>			m_loadedTextures;
	std::map<std::string, BitmapFont*>		m_loadedFonts;

	Shader* m_currentShader = nullptr;
};
