#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "engine/Math/vec2.hpp"
#include "Engine/Math/IntVec2.hpp"


struct AABB2;
class Entity;
class TileDefinition;


class Tile {
public:
	Tile(){}
	~Tile(){}
	Tile(const Tile& copyFrom);
	//explicit
	explicit Tile(int tileX, int tileY, TileDefinition* tileDef );
	explicit Tile( IntVec2& tileCoords, TileDefinition* tileDef );
	explicit Tile( int tileX, int tileY, std::string tileType );
	explicit Tile( IntVec2& tileCoords, std::string tileType );
	void Update(float deltaSeconds);
	void Render()const;
	void DebugRender() const;

	//Accessor
	bool		IsTagExist( const std::string& tag ) const;
	bool		IsTagsExist( const Strings& tags ) const;
	bool		IsTileType( std::string tileType ) const;
	bool		IsHeatBetween( FloatRange heatRange ) const;
	float		GetTileHeat() const;
	AABB2		GetBounds() const;
	std::string GetTileType() const;
	Strings		GetTileTags() const;
	TileDefinition* GetTileDef() const;
	std::string		GetTileTagsInString() const;

	//Mutator
	void SetTileType( const std::string& tileType );
	void SetTileTags( const Strings& tileTags );
	void InsertTileTag( const std::string& tileTag );
	bool DeleteTileTag( const std::string& tileTag );
	void SetTileHeat( float tileHeat );
	void AddTileHeat( float addHeat );
	void SubtractTileHeat( float subtractHeat );
	void MultiplyTileHeat( float multiplyHeat );
	void DivideTileHeat( float divideHeat );

public:	
	float			m_heat		= 0.f;
	IntVec2			m_tileCoords = IntVec2(0,0);
	TileDefinition* m_tileDef = nullptr;
	std::string		m_type		= "";
	Strings			m_tag;
};