#pragma once
#include <vector>
#include "Game/Entity.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"


class Game;
class World;
struct IntVec2;
class Entity;
class Player;
struct RaycastResult;

class Map {
public:
	Map(){}
	~Map();
	explicit Map( World* world, const IntVec2& tileDimensions);
	int GetTileIndexForTileCoords(const IntVec2& tileCoords) const;
	IntVec2 GetTileCoordsForTileIndex(const int tileIndex) const;
	void SetTileType(int tileX,int tileY, const TileType tileType);
	void Update(float deltaSeconds);
	void Render() const ;
	void DestroyEntities();
	void SpawnNewEntity(EntityType entityType, EntityFaction entityFaction, const Vec2& spawnPosition = Vec2::ONE);
	bool IsTileSolid(const IntVec2& tileCoords) const;
	RaycastResult Raycast(Vec2 start, Vec2 fwdDir, float maxDist);
	void HasLineOfSight();

private:
	bool IsTileCoordsValid(const IntVec2& tileCoords)const;
	bool IsTileIndexValid(int tileIndex) const;
	void RenderTiles() const;
	void PopulateTiles();
	void RenderEntities() const;
	void UpdateEntities(float deltaSeconds	);
	void UpdateCamera();
	void CheckCollision();
	void CheckPlayerTileCollisionWithTileCoords(Entity* player, IntVec2 tileCoords);
	void AddEntityToMap(Entity* entity);
	

private:
	World* m_world=nullptr;
	IntVec2 m_size=IntVec2();
	int m_tileNum=0;
	std::vector<Tile> m_tiles;
	EntityList m_entities[NUM_ENTITY_TYPES];
	//std::vector<Entity*> m_entities;//will remove
};