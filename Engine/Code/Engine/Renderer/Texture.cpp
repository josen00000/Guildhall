#include"Texture.hpp"



Texture::Texture(const unsigned int textureID, const char* imageFilePath, const float width, const float height )
	:m_textureID(textureID)
	,m_imageFilePath(imageFilePath)
	,m_width(width)
	,m_height(height)
{

}

void Texture::SetTextureID( int textureID )
{
	m_textureID=textureID;
}
