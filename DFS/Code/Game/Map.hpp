#pragma once
#include <string>
#include "Game/Game.hpp"
#include "Game/Tile.hpp"

class MapDefinition;

class Map {
public:
	Map(){}
	~Map();
	explicit Map( std::string name, MapDefinition* definition );
	static Map* CreateMap( std::string name, MapDefinition* definition );

public:
	void UpdateMap( float deltaSeconds );
	void RenderMap();

	// Accessor
	std::string GetName() const { return m_name; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	// Mutator
	void SetName( std::string name );
	void SetWidth( int width );
	void SetHeight( int height );
	void SetTileType( IntVec2 tileCoords, TileType type );
	void SetTileType( int tileX, int tileY, TileType type );

	// Tiles
	IntVec2 GetTileCoordsWithTileIndex( int tileIndex ) const;
	int GetTileIndexWithTileCoords( IntVec2 tileCoords ) const;
	bool IsTileCoordsValid( IntVec2 tileCoords ) const;

private:
	// Map Generation
	void CreateMapFromDefinition();
	void PopulateTiles();
	void InitializeTiles();
	void GenerateEdgeTiles( int edgeThick );
	void RunMapGenSteps();
	void PushTileVertices();

private:
	int								m_width = 0;
	int								m_height = 0;
	
	std::string						m_name = "";
	MapDefinition*					m_definition;
	std::vector<Tile>				m_tiles;
	std::vector<Vertex_PCU>			m_tilesVertices;
};