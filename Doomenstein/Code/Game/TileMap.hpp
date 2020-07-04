#pragma once
#include <vector>
#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

struct EntityInfo {
	Vec2 pos = Vec2::ZERO;
	float yaw = 0.f;
};

class TileMap : public Map {
public:
	TileMap(){}
	~TileMap(){}
	explicit TileMap( const XmlElement& mapElement, std::string name );
	//static Map* CreateTileMap();

public:
	virtual void RenderMap() const override;
	virtual void UpdateMeshes() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void PreparePlayer() override;
	virtual MapRaycastResult RayCast( Vec3 startPos, Vec3 forwardNormal, float maxDistance ) override;
	virtual MapRaycastResult RayCast( Vec3 startPos, Vec3 endPos );

	// collision
	virtual void CheckCollision() override;
	void CheckEntitiesCollision();
	void CheckTileCollision();
	void CheckCollisionBetweenTwoEntities( Entity* entityA, Entity* entityB );
	void CheckCollisionBetweenEntityAndTiles( Entity* entityA );
	void CheckEntityTileCollisionWithTileCoords( Entity* entity, IntVec2 tileCoords );

	Tile GetTileWithCoords( IntVec2 tileCoords )const;
	int GetTileIndexWithCoords( IntVec2 tileCoords )const;
	bool IsTileCoordsValid( IntVec2 tileCoords );
	bool IsTileCoordsSolid( IntVec2 tileCoords );

private:
	void LoadMapDefinitions( const XmlElement& mapElement );
	void EntityRaycast( MapRaycastResult& result );
	void TileRaycast( MapRaycastResult& result );
	FloatRange GetEntityXYRaycastDistRange( Entity* entity, Vec2 startPosXY, Vec2 forwardNormalXY, float maxDistXY );
	FloatRange GetEntityZRaycastDistRange( Entity* entity, float startZ, float maxDistZ );
	Vec3 GetTileXYRaycastNormalAndDist( Vec2 startPosXY, Vec2 forwardNormal, float maxDistXY );
	Vec2 GetTileZRaycastNormalAndDist( float startPosZ, float maxDistZ  );


	void DebugDrawRaycast() const;

	// validation
	void MapRowCharacterChecking( std::string mapRow );

	void CreateAndPopulateTiles();
	std::string GetRegionTypeFromMapRowsCoords( int tileX, int tileY );
	void AddVertsForSolidTile( std::vector<Vertex_PCU>& meshes, const Tile& tile );
	void AddVertsForNonSolidTile( std::vector<Vertex_PCU>& meshes, const Tile& tile );
	
	// entity
	void CreateEntities();

public:
	int			m_version = 1;
	IntVec2		m_dimensions = IntVec2::ZERO;
	IntVec2		m_mapRowDimensions = IntVec2::ZERO;
	std::string m_name = "";
	std::map<std::string, std::string> m_lengends;
	std::vector<std::string> m_mapRows;
	Vec2	m_playerStartPos = Vec2::ZERO; // temp without using entity
	float	m_playerStartYaw = 0.f;		   // temp without using entity
	std::vector<Vertex_PCU> m_meshes;
	std::vector<Tile> m_tiles;
	std::map<std::string, std::vector<EntityInfo>> m_actorList;
	std::map<std::string, std::vector<EntityInfo>> m_portalList;
	std::map<std::string, std::vector<EntityInfo>> m_projectileList;
	// raycast debug
	std::vector<Vertex_PCU> m_raycastDebugVerts;
};

