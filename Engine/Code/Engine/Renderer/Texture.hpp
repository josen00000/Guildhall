#pragma once
class Texture {
public:
	Texture(){}
	~Texture(){}
	Texture( const unsigned int textureID, const char* imageFilePath, const float width, const float height );

	void SetTextureID( const int textureID );
	const int GetTextureID() const { return m_textureID; } ;

public:
	unsigned int m_textureID	= 0;
	float m_width				= 0;
	float m_height				= 0;
	const char* m_imageFilePath = nullptr;

};