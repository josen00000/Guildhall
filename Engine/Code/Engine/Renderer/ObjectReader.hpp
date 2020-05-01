#pragma once
#include <vector>
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Core/Transform.hpp"


class GPUMesh;
struct Vertex_PCUTBN;


enum ObjType {
	BASIC_TYPE, // with normal , Pos, UV
};

enum PostProcessBit {
	POST_PROCESS_NONE			= 0,
	POST_INVERT_V_BIT			= 1,
	POST_INVERT_WIND_ORDER_BIT	= ( 1 << 1 ),
	POST_GENERATE_NORMAL_BIT	= ( 1 << 2 ),
	POST_APPLY_TRANSFORM_BIT	= ( 1 << 3 ),
};


class ObjectReader {
public:
	ObjectReader()=delete;
	~ObjectReader();
	explicit ObjectReader( std::string filePath, ObjType type=BASIC_TYPE );
	explicit ObjectReader( const char* filePath, ObjType type=BASIC_TYPE );

public:
	void GenerateVerticesAndIndices( std::vector<Vertex_PCUTBN>& vertices, std::vector<uint>& indices );
	void GenerateGPUMesh( GPUMesh& mesh );

	// mutator
	void SetInvertV( bool isAble );
	void SetGenerateNormal( bool isAble );
	void SetInvertWindOrder( bool isAble );
	void EnableTransform();
	void DisableTransform();
	void SetPostTrans( Transform trans );
	void SetPostTransPos( Vec3 pos );
	void SetPostTransRot( Vec3 rot );
	void SetPostTransScale( Vec3 scale );

private:
	// Accessor
	bool IsInvertV() const;
	bool IsGenerateNormal() const;
	bool IsInvertWindOrder() const;
	bool IsApplyTransform() const;
	void LoadObject();
	void ParseObjString( std::string line );
	void PostProcessing( std::vector<Vertex_PCUTBN>& vertices );
	void PostComputeNormal();
	void PostInvertV( std::vector<Vertex_PCUTBN>& vertices );
	void PostComputeTangent( std::vector<Vertex_PCUTBN>& vertices );
	void PostInvertWindOrder( std::vector<Vertex_PCUTBN>& vertices );
	void PostApplyTransform( std::vector<Vertex_PCUTBN>& vertices );

public:
	ObjType m_type;
	uint m_postState = 0;
	Transform m_trans;
	std::string m_filePath;
	std::vector<Vec3> m_positions;
	std::vector<Vec2> m_uvs;
	std::vector<Vec3> m_normals;
	std::vector<IntVec3> m_facePoints;
};

