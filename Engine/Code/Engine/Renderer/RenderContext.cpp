#pragma once

#include <vector>
#include "Engine/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "engine/Renderer/Texture.hpp"

void RenderContext::StartUp()
{
	
	
	
}
void RenderContext::BeginView(){

}



void RenderContext::BeginFrame()
{
	
}

void RenderContext::EndFrame()
{

}

void RenderContext::Shutdown()
{
	delete this;
	
}

void RenderContext::ClearScreen( const Rgba8& clearColor )
{
	UNUSED( clearColor );
// 	glClearColor( (float)clearColor.r/255, (float)clearColor.g/255, (float)clearColor.b/255, (float)clearColor.a/255 ); // Note; glClearColor takes colors as floats in [0,1], not bytes in [0,255]
// 	glClear( GL_COLOR_BUFFER_BIT ); // ALWAYS clear the screen at the top of each frame's Render()!
}

void RenderContext::BeginCamera( const Camera& camera )
{
	UNUSED( camera );
// 	glLoadIdentity();
// 	glOrtho( camera.m_AABB2.mins.x, camera.m_AABB2.maxs.x, camera.m_AABB2.mins.y, camera.m_AABB2.maxs.y, 0.f, 1.f );
}

void RenderContext::SetOrthoView( const AABB2& box )
{
	UNUSED( box ); 
// 	glLoadIdentity();
// 	glOrtho( box.mins.x, box.maxs.x, box.mins.y, box.maxs.y, 0.f, 1.f );
}
void RenderContext::EndCamera( const Camera& camera )
{
	UNUSED(camera);
	 //Vec2* useless= new Vec2( *camera.bot_left);// need to rewrite, tem for avoid warning
}


void RenderContext::BindTexture( const Texture* texture )
{
	UNUSED( texture );
	if( texture )
	{
// 		glEnable( GL_TEXTURE_2D );
// 		glBindTexture( GL_TEXTURE_2D, (GLuint)texture->GetTextureID() );
	}
	else
	{
//		glDisable( GL_TEXTURE_2D );
	}
}

Texture* RenderContext::CreateTextureFromFile( const char* imageFilePath )
{
	UNUSED( imageFilePath );

	//const char* imageFilePath = "Data/Images/Test_StbiFlippedAndOpenGL.png";
	unsigned int textureID = 0;
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );

	//int length = sizeof(*imageData)/sizeof(unsigned int);

	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ));
	GUARANTEE_OR_DIE( numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );
	// Enable OpenGL texturing
	//glEnable( GL_TEXTURE_2D );

	// Tell OpenGL that our pixel data is single-byte aligned
// 	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
// 
// 	// Ask OpenGL for an unused texName (ID number) to use for this texture
// 	glGenTextures( 1, (GLuint*)&textureID );
// 
// 	// Tell OpenGL to bind (set) this as the currently active texture
// 	glBindTexture( GL_TEXTURE_2D, textureID );
// 
// 	// Set texture clamp vs. wrap (repeat) default settings
// 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // GL_CLAMP or GL_REPEAT
// 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); // GL_CLAMP or GL_REPEAT
// 
// 																	// Set magnification (texel > pixel) and minification (texel < pixel) filters
// 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
// 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR
// 
// 																		// Pick the appropriate OpenGL format (RGB or RGBA) for this texel data
// 	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
// 	if( numComponents == 3 )
// 	{
// 		bufferFormat = GL_RGB;
// 	}
// 	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; technically allows us to translate into a different texture format as we upload to OpenGL
// 
// 										  // Upload the image texel data (raw pixels bytes) to OpenGL under this textureID
// 	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
// 		GL_TEXTURE_2D,		// Creating this as a 2d texture
// 		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
// 		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
// 		imageTexelSizeX,	// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
// 		imageTexelSizeY,	// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
// 		0,					// Border size, in texels (must be 0 or 1, recommend 0)
// 		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
// 		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
// 		imageData );		// Address of the actual pixel data bytes/buffer in system memory

							// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	Texture* temTexture = new Texture(textureID,imageFilePath,(float)imageTexelSizeX,(float)imageTexelSizeY);
	m_textureList.push_back(temTexture);					
	stbi_image_free( imageData );
	return temTexture;
}





Texture* RenderContext::CreateOrGetTextureFromFile( const char* imageFilePath )
{
	Texture* temTexture = CheckTextureExist(imageFilePath);
	if(temTexture != nullptr){
		return temTexture;
	}
	else{
		temTexture = CreateTextureFromFile(imageFilePath);
		return temTexture;
	}

}

BitmapFont* RenderContext::CreateOrGetBitmapFontFromFile( const char* fontName, const char* fontFilePath )
{
	BitmapFont* temFont = CheckBitmapFontExist(fontName);
	if(temFont != nullptr){ return temFont; }

	temFont = CreateBitmapFontFromFile(fontName, fontFilePath);
	return temFont;
}

BitmapFont* RenderContext::CreateBitmapFontFromFile( const char* fontName, const char* fontFilePath )
{
	std::string pngFilePath = fontFilePath;
	pngFilePath.append(".png");
	const char* temPath = pngFilePath.c_str();
	Texture* fontTexture = CreateOrGetTextureFromFile(temPath);
	BitmapFont* tem =  new BitmapFont(fontName, fontTexture);
	return tem;
}

BitmapFont* RenderContext::CheckBitmapFontExist( const char* imageFilePath ) const
{
	auto it =  m_loadedFonts.find(imageFilePath);
	if(it != m_loadedFonts.end() ){
		return it->second;
	}
	return nullptr;
}

Texture* RenderContext::CheckTextureExist(const char* imageFilePath) const
{
	for(int textureIndex=0;textureIndex<m_textureList.size();textureIndex++){
		if(strcmp(imageFilePath,m_textureList[textureIndex]->m_imageFilePath)==0){
			return m_textureList[textureIndex];
		}
	}
	return nullptr;
}

void RenderContext::DrawVertexVector( const std::vector<Vertex_PCU>& vertexArray )
{
	UNUSED( vertexArray );
// 	glBegin( GL_TRIANGLES ); {
// 		for( int vertexIndex =0; vertexIndex<vertexArray.size(); vertexIndex++ ) {
// 			const Vertex_PCU& temp_v=vertexArray[vertexIndex];
// 			glTexCoord2f(temp_v.m_uvTexCoords.x, temp_v.m_uvTexCoords.y);
// 			glColor4ub( temp_v.m_color.r, temp_v.m_color.g, temp_v.m_color.b, temp_v.m_color.a );
// 			glVertex3f( temp_v.m_pos.x, temp_v.m_pos.y, temp_v.m_pos.z );
// 		}
// 	}
// 	glEnd();
}

void RenderContext::DrawVertexArray( int vertexNum, Vertex_PCU* vertexArray )
{
	UNUSED( vertexArray );
	UNUSED( vertexNum );
// 	glBegin( GL_TRIANGLES ); {
// 		for( int vertexIndex =0; vertexIndex<vertexNum; vertexIndex++ ) {
// 			const Vertex_PCU& temp_v=vertexArray[vertexIndex];
// 			glTexCoord2f(temp_v.m_uvTexCoords.x, temp_v.m_uvTexCoords.y);
// 			glColor4ub( temp_v.m_color.r, temp_v.m_color.g, temp_v.m_color.b, temp_v.m_color.a );
// 			glVertex3f( temp_v.m_pos.x, temp_v.m_pos.y, temp_v.m_pos.z );
// 		}
// 	}
// 	glEnd();
}

void RenderContext::DrawAABB2D( const AABB2& bounds, const Rgba8& tint )
{
	//std::vector<Vertex_PCU> temAABB2;
	//temAABB2.push_back(Vertex_PCU)
	// 
	Vertex_PCU temAABB2[6]={
		// triangle2
		Vertex_PCU(Vec3(bounds.mins,0.f),tint,Vec2(0,0)),
		Vertex_PCU(Vec3(bounds.maxs.x,bounds.mins.y,0.f),tint,Vec2(1,0)),
		Vertex_PCU(Vec3(bounds.maxs,0.f),tint,Vec2(1,1)),
		// triangle2
		Vertex_PCU(Vec3(bounds.mins,0.f),tint,Vec2(0,0)),
		Vertex_PCU(Vec3(bounds.mins.x,bounds.maxs.y,0.f),tint,Vec2(0,1)),
		Vertex_PCU(Vec3(bounds.maxs,0.f),tint,Vec2(1,1)),
	};
	DrawVertexArray(6,temAABB2);
}

void RenderContext::DrawLine( const Vec2& startPoint, const Vec2& endPoint, const float thick, const Rgba8& lineColor )
{
	
	float halfThick=thick/2;
	Vec2 direction=endPoint.operator-( startPoint );
	direction.SetLength(halfThick);
	Vec2 tem_position=endPoint+direction;
	Vec2 rightTop=tem_position.operator+( Vec2(-direction.y,direction.x));
	Vec2 rightdown=tem_position.operator+(Vec2(direction.y,-direction.x));
	Vec2 tem_position1=startPoint-direction;
	Vec2 leftTop=tem_position1.operator+( Vec2( -direction.y, direction.x ) );
	Vec2 leftdown=tem_position1.operator+(  Vec2( direction.y, -direction.x ));
	Vec2 tem_uv=Vec2(0.f,0.f);
	Vertex_PCU line[6]={
		Vertex_PCU(  Vec3( rightTop.x,rightTop.y,0 ),Rgba8(lineColor.r,lineColor.g,lineColor.b),Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3(rightdown.x,rightdown.y,0), Rgba8(lineColor.r,lineColor.g,lineColor.b), Vec2( 0, 0 )), 
		Vertex_PCU( Vec3(leftTop.x,leftTop.y,0), Rgba8(lineColor.r,lineColor.g,lineColor.b), Vec2( 0, 0 ) ),
		Vertex_PCU(Vec3( leftTop.x,leftTop.y, 0 ), Rgba8(lineColor.r,lineColor.g,lineColor.b), Vec2( 0, 0 ) ),
		Vertex_PCU(Vec3( leftdown.x,leftdown.y, 0 ), Rgba8(lineColor.r,lineColor.g,lineColor.b), Vec2( 0, 0 ) ),
		Vertex_PCU(Vec3( rightdown.x,rightdown.y, 0 ), Rgba8(lineColor.r,lineColor.g,lineColor.b), Vec2( 0, 0 ))
	
	
	};


	DrawVertexArray(6,line);


}

void RenderContext::DrawCircle( Vec3 center,float radiu,float thick,Rgba8& circleColor )
{
	float degree=0;
	float nextDegree=0;
	for(int i=0;i<64;i++){
		nextDegree=(i+1)*(float)360/64;
		Vec2 startPoint= Vec2();
		startPoint.SetPolarDegrees(degree,radiu);
		startPoint.operator+=(Vec2(center));
		Vec2 endPoint= Vec2();
		endPoint.SetPolarDegrees(nextDegree,radiu);
		endPoint.operator+= ( Vec2(center));
		degree=nextDegree;
		DrawLine(startPoint,endPoint,thick,circleColor);
		
	}

}

void RenderContext::SetBlendMode( BlendMode blendMode )
{
	UNUSED( blendMode );
// 	if(blendMode == BlendMode::ALPHA){
// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 
// 	}
// 	else if(blendMode == BlendMode::ADDITIVE){
// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
// 	}
// 	else{
// 		ERROR_AND_DIE(Stringf( "Unknown / unsupported blend mode #%i", blendMode ));
// 	}
}

