#pragma once
#include <vector>
#include "Game/Entity.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Physics/RaycastResult.hpp"

class Game;
class World;
struct IntVec2;
class Entity;
class Player;
class NpcTurret;
class NpcTank;
class SpriteSheet;
class BitmapFont;
class SpriteAnimDefinition;

enum DeadSceneState {
	DEAD_SCENE_NULL,
	DEAD_SCENE_FADE_IN,
	DEAD_SCENE_WAIT_FOR_INPUT,
	DEAD_SCENE_FADE_OUT,
	NUM_DEAD_SCENE_STATE
};

class Map {
public:
	Map(){}
	~Map();
	explicit Map( World* world, const IntVec2& tileDimensions, std::vector<TileType> wormTypes, int* numWorm, int* wormLength, TileType edgeType, TileType defaultType);

	//basic
	void Render() const ;
	void Update(float deltaSeconds);

	//Tile
	bool IsTileOfType(const IntVec2& tileCoords, TileType tileType) const;
	bool IsTileBrick(const IntVec2& tileCoords) const;
	bool IsTileWater(const IntVec2& tileCoords) const;
	bool IsTileSolid(const IntVec2& tileCoords) const;
	bool IsTileWormed(const IntVec2& tileCoords ) const;
	bool IsTileEdge(const IntVec2& tileCoords) const;
	bool IsTileStart(const IntVec2& tileCoords) const;
	bool IsTileExit(const IntVec2& tileCoords) const;
	bool IsTileReachable(const IntVec2& tileCoords ) const;
	bool IsValidWormPos(const IntVec2 tileCoords) const;
	bool isValidCurrentWormPos(const IntVec2 tileCoords) const;

	//set
	void SetTileType(int tileX, int tileY, const TileType tileType);
	void SetTileType(IntVec2 tilePos, TileType tileType );
	void SetTileEdge(IntVec2 tilePos);
	void SetTileEdge(int tileX, int tileY);
	void SetWormTileType(IntVec2 tilePos, TileType tileType); // Change worm record when set the tile type
	void SetTileReachable(IntVec2 tilePos);
	void SetBrickTileToDeault(Vec2 point);

	//get	
	Vec2 GetNonSolidPos();
	int GetTileIndexForTileCoords(const IntVec2& tileCoords) const;
	IntVec2 GetTileCoordsForTileIndex(const int tileIndex) const;
	IntVec2 GetNextWormPos(IntVec2 currentPos);
	TileType GetTileTypeInWorld(const Vec2& worldCoords) const;
	TileType GetTileType(const IntVec2& tileCoords) const;
	
	//entity
	Entity* SpawnNewEntity(EntityType entityType, EntityFaction entityFaction, const Vec2& spawnPosition = Vec2::ONE);
	void SpawnNewBullet(EntityFaction entityFaction, const Vec2& spawnPosition, const Vec2& spawnDir);
	void SpawnNewExplosion(const Vec2& spawnPosition, float radiu, float duration, bool isExplosionInBrick = false );
	void DeleteEntity(Entity* entity);
	void DestroyEntities();
	void CheckEntitiesVisibility();
	void CheckVisibilityBetweenPlayerTurret(Player* player, NpcTurret* turret );
	void CheckVisibilityBetweenPlayerTank(Player* player, NpcTank* tank);
	
	//Physic
	bool isPointInSolid(const Vec2& point);
	bool IsPointInType(const Vec2& point, TileType tiletype);
	bool IsPointInBrick(const Vec2& point);
	bool IsPointInWater(const Vec2& point);
	bool IsPointInEntity(const Vec2& point , Entity* entity);
	RaycastResult Raycast(Vec2 start, Vec2 fwdDir, float maxDist);
	bool HasLineOfSight(Entity* entitySee, Entity* entitySeen, float maxDist);

	


	//Deadscene
	void InitializeDeadScene();
	void UpdatePlayerDeadScene(float deltaSeconds);
	void RenderPlayerDeadScene() const;

private:
	//basic
	void CheckRespawnPlayerInput();
	void CreateMap();
	void CreateEnemy();
	void UpdateEntities(float deltaSeconds	);
	void UpdateCamera();
	//void UpdateTestAnimation(float deltaSeconds);
	void RenderTiles()const;
	void RenderRaycastPoints() const;
	void RenderEntities() const;

	//tile
	void InitializeTiles();
	bool IsTileCoordsValid(const IntVec2& tileCoords)const;
	void PopulateTiles();
	void pushTileVertices();
	void GenerateWormTiles();
	void GenerateStartTiles();
	void GenerateExitTiles();
	void GenerateEdgeTiles();
	void UpdateSolidTiles();
	void UpdateBrickTiles();
	void UpdateBrickTilesToDefault();
	bool IsExitPosReachable();
	void UpdateReachableRecords();
	void FloodFillTiles(int tileX, int tileY);
	bool IsFloodFilledOnce(IntVec2 tileCoords);
	IntVec2 GenerateRandomValidTilePosInMap();

	//entity
	void CreateNewPlayer();
	void CheckIfPlayerExit();
	void CheckIfPlayerInLava();
	void CheckIfPlayerInMud();
	void CheckEntitiesDeath();
	void AddVertsForAABB2D(std::vector<Vertex_PCU>& vertices, AABB2 bound, Rgba8 tineColor, Vec2 uvAtMins, Vec2 uvAtMaxs);
	
	//physic
	void CheckCollision();
	void CheckEntitiesCollision();
	void CheckTileCollision();
	void CheckPlayerCollision();
	void CheckEnemyTankCollision(); //similar as check player collision
	void CheckTurretCollision();
	void CheckCollisionBetweenTwoEntities(Entity* a, Entity* b);
	void CheckBulletCollision();
	void CheckEntityTileCollisionWithTileCoords(Entity* entity, IntVec2 tileCoords);
	void AddEntityToMap(Entity* entity);
	
public:
	bool m_playerExit = false;
	bool m_isPlayerInMud =false;
	bool m_isPlayerInLava = false;
	World* m_world=nullptr;
	bool m_isPlayerDead = false;
	DeadSceneState m_deadSceneState = DEAD_SCENE_NULL;
	Vec2 m_playerDeadPos = Vec2::ZERO;

private:

	int m_tileNum=0;
	float m_currentTime = 0;
	float m_deadSceneFadeInTime = 0.f;
	float m_deadSceneFadeOutTime = 0.f;
	bool m_isValidMap = false;
	std::vector<Vertex_PCU> m_deadMessage;
	IntVec2 m_size=IntVec2();
	IntVec2 m_exitPos = IntVec2(0,0);
	IntVec2 m_startPos = IntVec2(2, 2);
	AABB2 m_deadScene ;
	Rgba8 m_deadSceneColor;
	SpriteSheet* m_tileSpriteSheet = nullptr;
	std::vector<Tile> m_tiles;
	std::vector<bool> m_wormTilesRecords;
	std::vector<bool> m_edgeTilesRecords;
	std::vector<bool> m_brickDestroiedRecords;
	std::vector<bool> m_reachableTilesRecords;
	std::vector<bool> m_solidTilesRecords;
	std::vector<bool> m_startTilesRecords;
	std::vector<bool> m_exitTilesRecords;
	std::vector<RaycastResult> m_raycastResult;
	EntityList m_entities[NUM_ENTITY_TYPES];
	std::vector<Vec2> m_raycastPoints;
	std::vector<Vertex_PCU> m_tileVertices;
	std::vector<Vertex_PCU> m_textVertices;
	std::vector<Vertex_PCU> m_animationVertices;
	SpriteSheet* m_animationSprite = nullptr;
	TileType m_edgeType = TileType::TILE_TYPE_NULL;
	TileType m_startType = TileType::TILE_TYPE_NULL;
	TileType m_exitType = TileType::TILE_TYPE_NULL;
	TileType m_defaultType = TileType::TILE_TYPE_NULL;
	std::vector<TileType> m_wormTypes;
	int* m_numWorms = nullptr;
	int* m_wormLength = nullptr;
	RandomNumberGenerator m_rng = RandomNumberGenerator();

	
};