#define STB_IMAGE_IMPLEMENTATION
#include "Image.hpp"
#include "Engine/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"


Image::Image( const char* imageFilePath )
	:m_imageFilePath(imageFilePath)
{
	CreateImage();
}

Image::Image( const std::vector<Rgba8>& imageData, const IntVec2& dimension, const char* imageFilePath/*="" */ )
	:m_dimensions(dimension)
{
	m_imageFilePath = std::string(imageFilePath);
	m_rgbaTexels = imageData;	
}

const std::string& Image::GetImageFinePath() const
{
	return m_imageFilePath;
}

IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}

std::vector<Rgba8> Image::GetImageData() const
{
	return m_rgbaTexels;
}

Rgba8 Image::GetTexelColor( int texelX, int texelY ) const
{
	int index = texelX + texelY * m_dimensions.x;
	return m_rgbaTexels[index];
}

Rgba8 Image::GetTexelColor( const IntVec2& texelCoords ) const
{
		int index = texelCoords.x + texelCoords.y * m_dimensions.x;
		return m_rgbaTexels[index];
}

void Image::CreateImage()
{
	int imageTexelSizeX = 0;
	int imageTexelSizeY = 0;
	int numComponents = 0;
	int numComponentsRequested = 0;

	//Load image from files
	stbi_set_flip_vertically_on_load(1);
	unsigned char* imageData = stbi_load(m_imageFilePath.c_str(), &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested);

	//Check if the load was successful
	GUARANTEE_OR_DIE(imageData, Stringf( "Failed to load image \"%s\"", m_imageFilePath.c_str() ));
	GUARANTEE_OR_DIE( numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", m_imageFilePath.c_str(), numComponents, imageTexelSizeX, imageTexelSizeY ) );

	m_dimensions.x = imageTexelSizeX;
	m_dimensions.y = imageTexelSizeY;

	unsigned char* imageTexel = imageData;
	int totalTexelNum = imageTexelSizeY * imageTexelSizeX;
	for( int texelIndex = 0; texelIndex < totalTexelNum; texelIndex++){
		unsigned char temR = *imageTexel;
		unsigned char temG = *(imageTexel + 1);
		unsigned char temB = *(imageTexel + 2);
		unsigned char temA;
		if(numComponents == 4){
			temA = *(imageTexel + 3);
			imageTexel = imageTexel + 4;
		}
		else{
			temA = 255;
			imageTexel = imageTexel + 3;
		}
		m_rgbaTexels.push_back(Rgba8(temR, temG, temB, temA));
	}
	stbi_image_free( imageData);
}

