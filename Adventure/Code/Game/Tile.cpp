#include "Tile.hpp"
#include "Game/App.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Renderer/RenderContext.hpp"


extern RenderContext* g_theRenderer;


Tile::Tile( const Tile& copyFrom )
	:m_tileCoords(copyFrom.m_tileCoords)
	,m_tileDef(copyFrom.m_tileDef)
{
	m_type = m_tileDef->m_name;
}

Tile::Tile( int tileX, int tileY, TileDefinition* tileDef )
	:m_tileDef(tileDef)
{
	m_tileCoords = IntVec2( tileX, tileY );
	m_type = m_tileDef->m_name;
}

Tile::Tile( IntVec2& tileCoords, TileDefinition* tileDef )
	:m_tileCoords(tileCoords)
	,m_tileDef(tileDef)
{
	m_type = m_tileDef->m_name;
}

Tile::Tile( int tileX, int tileY, std::string tileType )
{
	m_tileCoords = IntVec2( tileX, tileY );
	SetTileType( tileType );
}

Tile::Tile( IntVec2& tileCoords, std::string tileType )
{
	m_tileCoords = tileCoords;
	SetTileType( tileType );
}

void Tile::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Tile::Render()const 
{
}

void Tile::DebugRender() const
{

}

AABB2 Tile::GetBounds() const
{
	Vec2 mins( (float)m_tileCoords.x, (float)m_tileCoords.y );
	Vec2 maxs( mins.x+1.f, mins.y+1.f );
	return AABB2( mins, maxs );
}


TileDefinition* Tile::GetTileDef() const
{
	return m_tileDef;
}


std::string Tile::GetTileType() const
{
	return m_type;
}


bool Tile::IsTagExist( const std::string& tag ) const
{
	if( tag.compare("") == 0 ){ return true; }
	for( int tagIndex = 0; tagIndex < m_tag.size(); tagIndex++ ){
		if( m_tag[tagIndex].compare( tag ) == 0 ){
			return true;
		}
	}
	return false;
}

bool Tile::IsTagsExist( const Strings& tags ) const
{
	if( tags.empty() ){ return true; }
	for( int tagIndex = 0; tagIndex < tags.size(); tagIndex++ ){
		if( !IsTagExist( tags[tagIndex])){
			return false;
		}
	}
	return true;
}

bool Tile::IsTileType( std::string tileType ) const
{
	if( tileType.compare( "" ) == 0 || tileType.compare( m_type ) == 0 ){ return true; }
	else{ return false; }
}

bool Tile::IsHeatBetween( FloatRange heatRange ) const
{
	if( heatRange.IsFloatInRange( m_heat )){ return true; }
	else{ return false; }
}

Strings Tile::GetTileTags() const
{
	return m_tag;
}

std::string Tile::GetTileTagsInString() const
{
	std::string result = "";
	for(int tagIndex = 0; tagIndex < m_tag.size(); tagIndex++){
		result += m_tag[tagIndex];
	}
	return result;
}

float Tile::GetTileHeat() const
{
	return m_heat;
}

void Tile::SetTileType( const std::string& tileType )
{
	m_type = tileType;
	tileTypeDefMapIterator it = TileDefinition::s_definitions.find( tileType );
	m_tileDef = it->second;
}


void Tile::SetTileTags( const Strings& tileTags )
{
	m_tag = tileTags;
}

void Tile::InsertTileTag( const std::string& tileTag )
{
	m_tag.push_back( tileTag );
}

bool Tile::DeleteTileTag( const std::string& tileTag )
{
	for( int tagIndex = 0; tagIndex < m_tag.size(); tagIndex++ ){
		if( m_tag[tagIndex].compare( tileTag ) == 0 ){
			m_tag.erase( m_tag.begin() + tagIndex );
			return true;
		}
	}
	return false;
}

void Tile::SetTileHeat( float tileHeat )
{
	m_heat = tileHeat;
}

void Tile::AddTileHeat( float addHeat )
{
	m_heat += addHeat;
}

void Tile::SubtractTileHeat( float subtractHeat )
{
	m_heat -= subtractHeat;
}

void Tile::MultiplyTileHeat( float multiplyHeat )
{
	m_heat *= multiplyHeat;
}

void Tile::DivideTileHeat( float divideHeat )
{
	if( divideHeat == 0 ){
		ERROR_AND_DIE( "Divide heat can not be zero!!! ");
	}
	m_heat /= divideHeat;
}

