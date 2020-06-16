#pragma once
#include <vector>
#include "Game/Entity.hpp"
#include "Game/Tile.hpp"
#include "Game/Actor.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Physics/RaycastResult.hpp"

class Game;
class World;
class SpriteSheet;
class BitmapFont;
class SpriteAnimDefinition;
class MapGenStep;
class Mutator;
class Worm;
class FromImage;
//class CellularAutomata;
struct IntVec2;


struct TileAttributeInMap {
	TileAttributeInMap(){};
	~TileAttributeInMap(){};
	TileAttributeInMap(int tileIndex){
		m_tileIndex = tileIndex;
	}
	int m_tileIndex = 0;
	bool m_isEdge = false;
	bool m_isStart = false;
	bool m_isExit = false;
	bool m_isWormed = false;
	bool m_isReachable = false;
};


class Map {
public:
	Map(){}
	~Map(){}
	explicit Map( World* world, std::string name, MapDefinition* definition );
	void Render();
	void Update( float deltaSeconds);
	void DestroyEntities();

	//Accessor
	int			GetTileIndexWithTileCoords( const IntVec2& tileCoords ) const;
	Tile&		GetTileWithTileIndex( int tileIndex );
	Tile&		GetTileWithTileCoords(const IntVec2& tileCoords );
	bool		IsTileInside( const IntVec2& tileCoords ) const;
	bool		IsTileInType( const IntVec2& tileCoords, std::string tileType ) const;
	bool		IsTileCoordsValid(const IntVec2& tileCoords) const;
	bool		IsTileContainsTags( const IntVec2& tileCoords, Strings tags ) const;
	IntVec2		GetMapDimension() const;
	IntVec2		GetRandomValidTilePositionInMap();
	IntVec2		GetTileCoordsWithTileIndex(int index) const;
	std::vector<Tile>&			GetMapTiles();
	std::vector<std::string>	GetDisplayInfo() const;

	//Mutator
	void SetTileType( int tileX, int tileY, std::string tileType );
	void SetTileTags( int tileX, int tileY, Strings tags);
	void SetTileHeat( int tileX, int tileY, FloatRange heatRange );
	void OperateTileHeat( int tileX, int tileY, int operation, FloatRange operateRange );

private:
	// Map generation
	//////////////////////////////////////////////////////////////////////////
	void CreateMap();
	void PopulateTiles();
	void RunMapGenSteps();
	void InitializeTiles();
	void GenerateEdgeTiles(int edgeThick);
	void PushTileVertices();

	//Entity
	//////////////////////////////////////////////////////////////////////////
	void CreateActors();
	
	//Render
	//////////////////////////////////////////////////////////////////////////
	void RenderTiles();
	void AddVerticesForAABB2(std::vector<Vertex_PCU>& vertices, const AABB2& bound, const Rgba8& tintColor, const Vec2 minUV, const Vec2 maxUV);

	//Update
	//////////////////////////////////////////////////////////////////////////
	void UpdateMousePosInCamera( const Camera& camera);

	//Accessor
	//////////////////////////////////////////////////////////////////////////
	std::string GetDisplayTileInfo( int tileIndex ) const;
	bool IsTileInEdge( const IntVec2& tileCoords ) const;

public:
	World* m_world = nullptr;
	RandomNumberGenerator m_rng = RandomNumberGenerator();
	
private:
	Vec2 m_mousePos;
	std::string m_name = "";
	MapDefinition*  m_definition;
	std::vector<Tile> m_tiles;
	std::vector<Vertex_PCU> m_tilesVertices;
	std::vector<TileAttributeInMap> m_tileMapAttributes;
	int m_width = 0;
	int m_height = 0;
	std::vector<Actor*> m_actors;
};