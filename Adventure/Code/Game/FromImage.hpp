#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine/Core/Rgba8.hpp"
#include "Game/Tile.hpp"
#include "Game/MapGenStep.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/IntRange.hpp"

class Image;

//typedef std::map<Rgba8, TileType> ColorTileTypeMap;
//typedef ColorTileTypeMap::const_iterator ColorTileTypeMapIterator;

class FromImage: public MapGenStep
{
public:
	FromImage()=default;
	~FromImage()=default;
	explicit FromImage( const XmlElement& FromImageElement );
	virtual void RunStepOnce( Map* map ) const override;
	//static std::map<Rgba8, TileDefinition> s_colorTileDefMap; //Error with reason not understand
private:
	std::vector<Rgba8> CreateOrientedImage() const;
	std::vector<Rgba8> GetOrientedImageData() const;
	IntVec2 GetMapTileCoords( int texelIndex, const IntVec2& alignedVec2 ) const;
	IntVec2 GetAlignmentTranslation( Map* map ) const;
	IntVec2 GetOrientedCoords( const IntVec2& coords, const IntVec2& transform, const IntVec2& rotation  ) const;
	IntVec2 GetTexelCoordsWithIndex( int index, const IntVec2& dimension ) const;
	IntVec2 GetOrientedDimension() const;
	int GetTexelIndexWithCoords( const IntVec2& texelCoords, const IntVec2& dimension ) const;
	const TileDefinition* GetTileDefinitionWithColor( const Rgba8& color ) const;

public:
	Image*			m_image = nullptr;
	float			m_chancePerTile = 1.f;
	int				m_orientation = 0;
	Vec2			m_alignment = Vec2::ZERO;
	IntVec2			m_dimension = IntVec2( 0, 0 );
	std::string		m_dataFile = "";
};