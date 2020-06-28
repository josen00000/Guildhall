#pragma once
#include <vector>
#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class TileMap : public Map {
public:
	TileMap(){}
	~TileMap(){}
	explicit TileMap( const XmlElement& mapElement );
	//static Map* CreateTileMap();

public:
	virtual void RenderMap() const override;
	virtual void UpdateMeshes() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void PrepareCamera() override;
	virtual MapRaycastResult RayCast( Vec3 startPos, Vec3 forwardNormal, float maxDistance ) override;
	virtual MapRaycastResult RayCast( Vec3 startPos, Vec3 endPos );


	Tile GetTileWithCoords( IntVec2 tileCoords )const;
	int GetTileIndexWithCoords( IntVec2 tileCoords )const;
	bool IsTileCoordsValid( IntVec2 tileCoords );
	bool IsTileCoordsSolid( IntVec2 tileCoords );

private:
	void LoadMapDefinitions( const XmlElement& mapElement );
	void EntityXYRaycast( MapRaycastResult& result );
	void EntityZRaycast( MapRaycastResult& result );
	float GetTileXYRaycastDist( Vec2 startPosXY, Vec2 forwardNormalXY, float maxDistXY );
	float GetTileZRaycastDist( float startPosZ, float maxDistZ  );
	MapRaycastResult GetIntegrateResults( MapRaycastResult* results );


	void DebugDrawRaycast() const;

	// validation
	void MapRowCharacterChecking( std::string mapRow );

	void CreateAndPopulateTiles();
	std::string GetRegionTypeFromMapRowsCoords( int tileX, int tileY );
	void AddVertsForSolidTile( std::vector<Vertex_PCU>& meshes, const Tile& tile );
	void AddVertsForNonSolidTile( std::vector<Vertex_PCU>& meshes, const Tile& tile );
	
public:
	int			m_version = 1;
	IntVec2		m_dimensions = IntVec2::ZERO;
	IntVec2		m_mapRowDimensions = IntVec2::ZERO;
	std::map<std::string, std::string> m_lengends;
	std::vector<std::string> m_mapRows;
	Vec2	m_playerStartPos = Vec2::ZERO; // temp without using entity
	float	m_playerStartYaw = 0.f;		   // temp without using entity
	std::vector<Vertex_PCU> m_meshes;
	std::vector<Tile> m_tiles;

	// raycast debug
	std::vector<Vertex_PCU> m_raycastDebugVerts;
};

