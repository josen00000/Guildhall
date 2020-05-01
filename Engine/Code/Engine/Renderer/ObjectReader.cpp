#include <fstream>
#include "ObjectReader.hpp"
#include "Engine/ThirdParty/mikktspace.h"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/MeshUtils.hpp"


ObjectReader::ObjectReader( const char* filePath, ObjType type )
	:m_filePath(filePath)
	,m_type(type)
{
	LoadObject();
	m_trans = Transform();
}

ObjectReader::ObjectReader( std::string filePath, ObjType type )
	:m_filePath(filePath)
	,m_type(type)
{
	LoadObject();
	m_trans = Transform();
}

void ObjectReader::GenerateVerticesAndIndices( std::vector<Vertex_PCUTBN>& vertices, std::vector<uint>& indices )
{
	std::vector<Vertex_PCUTBN> localVertices;
	Vec3 normal;
	for( int i = 0; i < m_facePoints.size(); i++ ) {
		Vertex_PCUTBN temp;
		int posIndex	= m_facePoints[i].x - 1;
		int uvIndex		= m_facePoints[i].y - 1;
		int normalIndex = m_facePoints[i].z - 1;

		temp.m_pcu.m_pos			= m_positions[posIndex];
		temp.m_pcu.m_uvTexCoords	= m_uvs[uvIndex];
		temp.m_tangent				= Vec3::ONE;		// temp 
		temp.m_bitangent			= Vec3::ONE;	// temp 
		
		if( IsInvertV() ) {
			temp.m_pcu.m_uvTexCoords *= -1.f;
		}
		if( IsGenerateNormal() ) {
			if( i % 3 == 0 ) {
				int posIndexB = m_facePoints[i+1].x - 1;
				int posIndexC = m_facePoints[i+2].x - 1;
				Vec3 AB = m_positions[posIndexB] - temp.m_pcu.m_pos;
				Vec3 AC = m_positions[posIndexC] - temp.m_pcu.m_pos;
				temp.m_normal = CrossProduct3D( AB, AC ).GetNormalized();
				normal = temp.m_normal;
			}
			else {
				temp.m_normal = normal;
			}
		}
		else {
			temp.m_normal = m_normals[normalIndex];

		}

		localVertices.push_back( temp );
	}
	PostProcessing( localVertices );
	AppendIndexedTBNVerts( vertices, indices, localVertices );
}

void ObjectReader::GenerateGPUMesh( GPUMesh& mesh )
{
	std::vector<Vertex_PCUTBN> vertices;
	std::vector<uint> indices;
	GenerateVerticesAndIndices( vertices, indices );
	mesh.UpdateTBNVerticesInCPU( vertices );
	mesh.UpdateIndicesInCPU( indices );
}

void ObjectReader::SetInvertV( bool isAble )
{
	if( isAble ) {
		m_postState = m_postState | POST_INVERT_V_BIT;
	}
	else {
		m_postState = m_postState & ~POST_INVERT_V_BIT;
	}
}

void ObjectReader::SetGenerateNormal( bool isAble )
{
	if( isAble ) {
		m_postState = m_postState | POST_GENERATE_NORMAL_BIT;
	}
	else {
		m_postState = m_postState & ~POST_GENERATE_NORMAL_BIT;
	}
}

void ObjectReader::SetInvertWindOrder( bool isAble )
{
	if( isAble ) {
		m_postState = m_postState | POST_INVERT_WIND_ORDER_BIT;
	}
	else {
		m_postState = m_postState & ~POST_INVERT_WIND_ORDER_BIT;
	}
}

void ObjectReader::EnableTransform()
{
	m_postState = m_postState | POST_APPLY_TRANSFORM_BIT;
}

void ObjectReader::DisableTransform()
{
	m_postState = m_postState & ~POST_APPLY_TRANSFORM_BIT;
}

void ObjectReader::SetPostTrans( Transform trans )
{
	m_trans = trans;
	EnableTransform();
}

void ObjectReader::SetPostTransPos( Vec3 pos )
{
	m_trans.SetPosition( pos );
	EnableTransform();
}

void ObjectReader::SetPostTransRot( Vec3 rot )
{
	m_trans.SetRotationFromPitchRollYawDegrees( rot );
	EnableTransform();
}

void ObjectReader::SetPostTransScale( Vec3 scale )
{
	m_trans.SetScale( scale );
	EnableTransform();
}

bool ObjectReader::IsInvertV() const
{
	return ( m_postState & POST_INVERT_V_BIT );
}

bool ObjectReader::IsGenerateNormal() const
{
	return ( m_postState & POST_GENERATE_NORMAL_BIT );
}

bool ObjectReader::IsInvertWindOrder() const
{
	return ( m_postState & POST_INVERT_WIND_ORDER_BIT );
}

bool ObjectReader::IsApplyTransform() const
{
	return ( m_postState & POST_APPLY_TRANSFORM_BIT );
}

void ObjectReader::LoadObject()
{
	std::ifstream objfile;
	objfile.open( m_filePath );

	// load raw data
	if( objfile.is_open() ) {
		std::string line;
		while( getline( objfile, line ) ) {
			if( IsStringFullOfSpace( line ) ){ continue; }
			if( IsStringStartWithChar( line, '#' ) ){ continue; }
			ParseObjString( line );
		}
	}
	objfile.close();
}

void ObjectReader::ParseObjString( std::string line )
{
	Strings lineData = SplitStringOnDelimiter( line, ' ' );
	if( lineData[0].compare( "v" ) == 0 ){
		Vec3 pos;
		pos.x = (float)atof( lineData[1].c_str() );
		pos.y = (float)atof( lineData[2].c_str() );
		pos.z = (float)atof( lineData[3].c_str() );
		m_positions.push_back( pos );
	}
	else if( lineData[0].compare( "vn" ) == 0 ){
		Vec3 normal;
		normal.x = (float)atof( lineData[1].c_str() );
		normal.y = (float)atof( lineData[2].c_str() );
		normal.z = (float)atof( lineData[3].c_str() );
		m_normals.push_back( normal );
	}
	else if( lineData[0].compare( "vt" ) == 0 ) {
		Vec2 uv;
		uv.x = (float)atof( lineData[1].c_str() );
		uv.y = (float)atof( lineData[2].c_str() );
		m_uvs.push_back( uv );
	}
	else if( lineData[0].compare( "f" ) == 0 ){
		if( lineData.size() == 4 ) {
			for( int i = 1; i < lineData.size(); i++ ) {
				IntVec3 facePoint;
				Strings points = SplitStringOnDelimiter( lineData[i], '/' );
				facePoint.x = stoi( points[0] );
				facePoint.y = stoi( points[1] );
				if( points.size() == 3 ) {
					facePoint.z = stoi( points[2] );
				}
				else {
					if( !IsGenerateNormal() ) {
						SetGenerateNormal( true );
					}
				}
				m_facePoints.push_back( facePoint );
			}
		}
		else if( lineData.size() == 5 ){
			for( int i = 1; i < 4; i++ ) {
				IntVec3 facePoint;
				Strings points = SplitStringOnDelimiter( lineData[i], '/' );
				facePoint.x = stoi( points[0] );
				facePoint.y = stoi( points[1] );
				if( points.size() == 3 ) {
					facePoint.z = stoi( points[2] );
				}
				else {
					if( !IsGenerateNormal() ) {
						SetGenerateNormal( true );
					}
				}
				m_facePoints.push_back( facePoint );
			}
			for( int i = 1; i < 5; i++ ) {
				if( i == 2 ){ continue; }
				IntVec3 facePoint;
				Strings points = SplitStringOnDelimiter( lineData[i], '/' );
				facePoint.x = stoi( points[0] );
				facePoint.y = stoi( points[1] );
				if( points.size() == 3 ) {
					facePoint.z = stoi( points[2] );
				}
				m_facePoints.push_back( facePoint );
			}
		}
	}
}

void ObjectReader::PostProcessing( std::vector<Vertex_PCUTBN>& vertices )
{
	PostComputeTangent( vertices );
	PostApplyTransform( vertices );
}

void ObjectReader::PostComputeNormal()
{

}

void ObjectReader::PostInvertWindOrder( std::vector<Vertex_PCUTBN>& vertices )
{
	for( int i = 0; i < vertices.size(); i++ ) {
		if( i % 3 == 0 ) {
			Vertex_PCUTBN temp = vertices[i+1];
			vertices[i+1] = vertices[i+2];
			vertices[i+2] = temp;
		}
	}
}


void ObjectReader::PostApplyTransform( std::vector<Vertex_PCUTBN>& vertices )
{
	if( !IsApplyTransform() ){ return; }

	Mat44 transMat = m_trans.ToMatrix();
	for( int i = 0; i < vertices.size(); i++ ) {
		vertices[i].m_pcu.m_pos = transMat.TransformPosition3D( vertices[i].m_pcu.m_pos );
		vertices[i].m_normal	= transMat.TransformVector3D( vertices[i].m_normal );
		vertices[i].m_bitangent = transMat.TransformVector3D( vertices[i].m_bitangent );
		vertices[i].m_tangent	= transMat.TransformVector3D( vertices[i].m_tangent );
	}
}

void ObjectReader::PostInvertV( std::vector<Vertex_PCUTBN>& vertices )
{
	for( int i = 0; i < vertices.size(); i++ ) {
		vertices[i].m_pcu.m_uvTexCoords.y *= -1;
	}
}

static int GetNumFaces( const SMikkTSpaceContext* context ) 
{
	const std::vector<Vertex_PCUTBN>* vertices = (std::vector<Vertex_PCUTBN>*)context->m_pUserData;
	return (int)(vertices->size() / 3);
}


static int GetNumVerticesOfFace( const SMikkTSpaceContext* context, const int face ) 
{
	UNUSED(context);
	UNUSED(face);
	return 3;
}

static void GetPositionForFaceVert( const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert ) 
{
	const std::vector<Vertex_PCUTBN>* vertices = (std::vector<Vertex_PCUTBN>*)pContext->m_pUserData;
	int indexInVertexArray = iFace * 3 + iVert;

	Vec3 outPos = (*vertices)[indexInVertexArray].m_pcu.m_pos;
	fvPosOut[0] = outPos.x;
	fvPosOut[1] = outPos.y;
	fvPosOut[2] = outPos.z;
}

static void GetNormalForFaceVert( const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert ) 
{
	const std::vector<Vertex_PCUTBN>* vertices = (std::vector<Vertex_PCUTBN>*)pContext->m_pUserData;
	int indexInVertexArray = iFace * 3 + iVert;

	Vec3 outNormal = (*vertices)[indexInVertexArray].m_normal;

	fvNormOut[0] = outNormal.x;
	fvNormOut[1] = outNormal.y;
	fvNormOut[2] = outNormal.z;
}

static void GetUVForFaceVert( const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert ) 
{
	const std::vector<Vertex_PCUTBN>* vertices = (std::vector<Vertex_PCUTBN>*)pContext->m_pUserData;
	int indexInVertexArray = iFace * 3 + iVert;

	Vec3 outUV = (*vertices)[indexInVertexArray].m_pcu.m_uvTexCoords;

	fvTexcOut[0] = outUV.x;
	fvTexcOut[1] = outUV.y;
}

static void SetTangent( const SMikkTSpaceContext * pContext,
						const float fvTangent[],
						const float fSign,
						const int iFace,
						const int iVert ) {
	std::vector<Vertex_PCUTBN>* vertices = (std::vector<Vertex_PCUTBN>*)pContext->m_pUserData;
	int indexInVertexArray = iFace * 3 + iVert;

	Vertex_PCUTBN& vert = (*vertices)[indexInVertexArray];
	vert.m_tangent = Vec3( fvTangent[0], fvTangent[1], fvTangent[2] );

	vert.m_bitangent = CrossProduct3D( vert.m_normal, vert.m_tangent ) * fSign;
}

void ObjectReader::PostComputeTangent( std::vector<Vertex_PCUTBN>& vertices )
{
	SMikkTSpaceInterface mikktInterface;
	mikktInterface.m_getNumFaces = GetNumFaces;
	mikktInterface.m_getNumVerticesOfFace = GetNumVerticesOfFace;

	mikktInterface.m_getPosition = GetPositionForFaceVert;
	mikktInterface.m_getTexCoord = GetUVForFaceVert;
	mikktInterface.m_getNormal = GetNormalForFaceVert;

	mikktInterface.m_setTSpaceBasic = SetTangent;
	mikktInterface.m_setTSpace = nullptr;

	SMikkTSpaceContext context;
	context.m_pInterface = &mikktInterface;
	context.m_pUserData = &vertices;

	genTangSpaceDefault( &context );
}


