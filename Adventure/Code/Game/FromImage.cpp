#include "FromImage.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/core/Image.hpp"
#include "Engine/Core/XmlUtils.hpp"


FromImage::FromImage( const XmlElement& FromImageElement )
	:MapGenStep(FromImageElement)
{
	m_chancePerTile		= ParseXmlAttribute( FromImageElement, "chancePerTile", 1.f );
	m_orientation		= ParseXmlAttribute( FromImageElement, "orientation", 1 );
	float alignmentX	= ParseXmlAttribute( FromImageElement, "alignmentX", 0.f);
	float alignmentY	= ParseXmlAttribute( FromImageElement, "alignmentY", 0.f);
	m_dataFile			= ParseXmlAttribute( FromImageElement, "imageFile", "");;
	m_alignment			= Vec2( alignmentX, alignmentY );
	m_image				= new Image( m_dataFile.c_str() );
	m_dimension			= m_image->GetDimensions();
 
}

void FromImage::RunStepOnce( Map* map ) const
{
	std::vector<Rgba8> orientedImageData =	CreateOrientedImage();
	IntVec2 alignmentTranslation = GetAlignmentTranslation( map );
	for(int texelIndex = 0; texelIndex < orientedImageData.size(); ++texelIndex ){
		IntVec2 mapTileCoords = GetMapTileCoords( texelIndex, alignmentTranslation );
		Rgba8 tempTexelElement = orientedImageData[texelIndex];
		if(tempTexelElement.a == 0 ){
			continue;
		}
		if(!map->IsTileCoordsValid(mapTileCoords)){
			continue;
		}
		const TileDefinition* tempTileDef = GetTileDefinitionWithColor( tempTexelElement );
		if( tempTileDef ){
			std::string setTileType = tempTileDef->m_name; 
			map->SetTileType( mapTileCoords.x, mapTileCoords.y, setTileType );
		}
	}
}



std::vector<Rgba8> FromImage::CreateOrientedImage() const
{
	std::vector<Rgba8> resultImageData = GetOrientedImageData();
	return resultImageData;
}

std::vector<Rgba8> FromImage::GetOrientedImageData() const
{
	IntVec2 translateVec2;
	IntVec2 rotationVec2;
	std::vector<Rgba8> originalImageData = m_image->GetImageData();
	std::vector<Rgba8> resultImageData = originalImageData;

	IntVec2 tempDimension = m_dimension - IntVec2( 1, 1 );
	if( m_orientation == 0 ) { return m_image->GetImageData(); }
	else if( m_orientation == 1 ) {
		translateVec2 = IntVec2( tempDimension.y, 0 );
		rotationVec2 = IntVec2( 0, 1 );
	}
	else if( m_orientation == 2 ) {
		translateVec2 = IntVec2( tempDimension.x, tempDimension.y );
		rotationVec2 = IntVec2( -1, 0 );
	}
	else /*if( m_orientation == 3 )*/ {
		translateVec2 = IntVec2( 0, tempDimension.x );
		rotationVec2 = IntVec2( 0, -1 );
	}
	IntVec2  orientedDimension = GetOrientedDimension();
	for( int texIndex = 0; texIndex < originalImageData.size(); texIndex++ ) {
		IntVec2 inputCoords = GetTexelCoordsWithIndex( texIndex, m_dimension );
		IntVec2 orientedCoords = GetOrientedCoords( inputCoords, translateVec2, rotationVec2 );
		int orientedIndex = GetTexelIndexWithCoords( orientedCoords, orientedDimension );
		resultImageData[orientedIndex] = originalImageData[texIndex];
	}
	return resultImageData;
}

IntVec2 FromImage::GetMapTileCoords( int texelIndex, const IntVec2& alignedVec2 ) const
{
	IntVec2 orientedDimension;
	if( m_orientation ==1 || m_orientation == 3 ){
		orientedDimension = IntVec2( m_dimension.y, m_dimension.x );
	}
	else{
		orientedDimension = m_dimension;
	}
	IntVec2 texelCoords = GetTexelCoordsWithIndex(texelIndex, orientedDimension );
	IntVec2 mapTileCoords = texelCoords + alignedVec2;
	return mapTileCoords;
}

IntVec2 FromImage::GetAlignmentTranslation( Map* map ) const
{
	IntVec2 orientedDimension;
	if( m_orientation == 1 || m_orientation == 3){
		orientedDimension = IntVec2( m_dimension.y, m_dimension.x );
	}
	else{
		orientedDimension = m_dimension;
	}
	IntVec2 maxAlignmentPos = map->GetMapDimension() - orientedDimension;
	float alignedPosx = RangeMapFloat( 0, 1, 0, (float)maxAlignmentPos.x, m_alignment.x );
	float alignedPosy = RangeMapFloat( 0, 1, 0, (float)maxAlignmentPos.y, m_alignment.x );
	IntVec2 result = IntVec2( (int)alignedPosx, (int)alignedPosy );
	return result;
}

IntVec2 FromImage::GetOrientedCoords( const IntVec2& coords, const IntVec2& transform, const IntVec2& rotation ) const
{
	int resultTexelX = (rotation.x * coords.x) - (rotation.y * coords.y) + transform.x;
	int resultTexelY = (rotation.y * coords.x) + (rotation.x * coords.y) + transform.y;
	IntVec2 result = IntVec2( resultTexelX, resultTexelY );
	return result;
}

IntVec2 FromImage::GetTexelCoordsWithIndex( int index, const IntVec2& dimension ) const
{
	int texelX = index % dimension.x;
	int texelY = index / dimension.x;
	IntVec2 result = IntVec2( texelX, texelY );
	return result;
}

IntVec2 FromImage::GetOrientedDimension() const
{
	IntVec2 orientedDimension;
	if( m_orientation == 1 || m_orientation == 3 ) {
		orientedDimension = IntVec2( m_dimension.y, m_dimension.x );
	}
	else {
		orientedDimension = m_dimension;
	}
	return orientedDimension;

}

int FromImage::GetTexelIndexWithCoords( const IntVec2& texelCoords, const IntVec2& dimension ) const
{
	int texelIndex = ( texelCoords.y * dimension.x ) + texelCoords.x;
	return texelIndex;
}

const TileDefinition* FromImage::GetTileDefinitionWithColor( const Rgba8& color ) const
{
	tileTypeDefMap& tempMap = TileDefinition::s_definitions;
	for( tileTypeDefMapIterator it = tempMap.begin(); it != tempMap.end(); ++it ) {
		const TileDefinition* tempValue = it->second;
		Rgba8 tileColor = tempValue->m_fromImageColor;
		if( color.r == tileColor.r && color.g == tileColor.g && color.b == tileColor.b ){
			return tempValue;
		}
	}
	return nullptr;
}
