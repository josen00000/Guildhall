#pragma once
#include <string>
#include <vector>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"

class Texture;
struct Rgba8;

class Image {
public:
	Image( const char* imageFilePath);
	Image( const std::vector<Rgba8>& imageData, const IntVec2& dimension, const char* imageFilePath="" );
	const std::string&		GetImageFinePath() const;
	IntVec2					GetDimensions() const;
	std::vector<Rgba8>		GetImageData() const;
	Rgba8					GetTexelColor( int texelX, int texelY) const;
	Rgba8					GetTexelColor( const IntVec2& texelCoords) const;
	void					CreateImage();

private:
	std::string			m_imageFilePath;
	IntVec2				m_dimensions = IntVec2(0,0);
	std::vector<Rgba8>	m_rgbaTexels;


};