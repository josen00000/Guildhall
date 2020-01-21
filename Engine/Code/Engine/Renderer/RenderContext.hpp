#pragma once
//#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
//#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library
#include <map>
#include <string>
#include "Engine/Core/Vertex_PCU.hpp"

class Camera;
class Texture;
class BitmapFont;
struct AABB2;

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
	void StartUp();
	void BeginView();
	void BeginFrame();
	void EndFrame();
	void Shutdown();
	void ClearScreen(const Rgba8& clearColo);
	void BeginCamera(const Camera& camera);
	void SetOrthoView(const AABB2& box);
	void EndCamera(const Camera& camera);
	void BindTexture(const Texture* texture);
	void DrawVertexVector(const std::vector<Vertex_PCU>& vertexArray);
	void DrawVertexArray(int vertexNum, Vertex_PCU* vertexArray);
	void DrawAABB2D(const AABB2& bounds,const Rgba8& tint);
	void DrawLine(const Vec2& startPoint,const Vec2&endPoint,const float thick,const Rgba8& lineColor);
	void DrawCircle(Vec3 center,float radiu,float thick,Rgba8& circleColor);
	void SetBlendMode(BlendMode blendMode);
	Texture* CreateOrGetTextureFromFile(const char* imageFilePath);
	BitmapFont* CreateOrGetBitmapFontFromFile(const char* fontName, const char* fontFilePath);

private:
	Texture*		CreateTextureFromFile(const char* imageFilePath);
	BitmapFont*		CreateBitmapFontFromFile(const char* fontName, const char* fontFilePath);
	Texture*		CheckTextureExist(const char* imageFilePath) const;
	BitmapFont*		CheckBitmapFontExist( const char* fontName ) const;



private:
	std::vector <Texture*> m_textureList;
	std::map<std::string, Texture*>			m_loadedTextures;
	std::map<std::string, BitmapFont*>		m_loadedFonts;

};
