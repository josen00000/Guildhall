//#include "common.hlsl"
#include "phong.hlsl"
//--------------------------------------------------------------------------------------
// Stream Input
// ------
// Stream Input is input that is walked by the vertex shader.
// If you say "Draw(3,0)", you are telling to the GPU to expect '3' sets, or
// elements, of input data.  IE, 3 vertices.  Each call of the VertxShader
// we be processing a different element.
//--------------------------------------------------------------------------------------

// inputs are made up of internal names (ie: uv) and semantic names
// (ie: TEXCOORD).  "uv" would be used in the shader file, where
// "TEXCOORD" is used from the client-side (cpp code) to attach ot.
// The semantic and internal names can be whatever you want,
// but know that semantics starting with SV_* usually denote special
// inputs/outputs, so probably best to avoid that naming.
struct vs_input_t
{
	// we are not defining our own input data;
	float3 position		: POSITION;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT;
	float3 bitangent	: BITANGENT;
	float3 normal		: NORMAL;
};

cbuffer material_constants : register(b6) {
	uint STEP_COUNT;
	float DEPTH;
	float2 pad;
}


Texture2D <float4> tDiffuse		: register(t0);
Texture2D <float4> tNormal		: register(t4);
Texture2D <float4> tMaterial	: register(t8);
SamplerState sSampler: register(s0);

float2 ComputeShallowParallaxUV( float3 local_dir, float2 uv_start ) {
	float2 uv_move = local_dir.xy * float2( DEPTH, DEPTH ) / abs(local_dir.z);
	float2 uv_end = uv_start + uv_move;
	float h0 = tMaterial.Sample( sSampler, uv_start ).x;
	float h1 = tMaterial.Sample( sSampler, uv_end ).x;
	float t = ( 1 - h0 ) / ( ( h1 - h0 ) + 1 );
	//return t.xx * uv_move + float2( 0.5f, 0.5f );
	return lerp( uv_start, uv_end, t.xx );
}

float2 ComputeDeepParallaxUV( float3 local_dir, float2 uv_start ) {
	float2 step_uv = uv_start;

	const float step = 1.0f / STEP_COUNT;
	float2 uv_move = local_dir.xy * DEPTH * step / abs(local_dir.z); // why 0.5 here

	float z = 1.0f;
	float height = 0.f;

	int i = 0;
	float heights[32];
	for( i = 0; i < STEP_COUNT; i++ ) {
		heights[i] = tMaterial.Sample( sSampler, step_uv ).x;
		step_uv += uv_move;
	}

	for( int i = 1; i < STEP_COUNT; i++ ) {
		float h1 = heights[i];
		if( (z - step) <= h1 ) {
			float t0 = z;
			float t1 = z - step;
			float h0 = heights[i - 1];

			float t = ( t0 - h0 ) / ( h1 - h0 + t0 - t1 );
			float2 uv0 = uv_start + uv_move * ( i - 1 );
			float2 uv1 = uv0 + uv_move;
			return lerp( uv0, uv1, t.xx );
		}
		z -= step;
	}
	return float2( 1.f, 1.f );
}


//--------------------------------------------------------------------------------------
// Programmable Shader Stages
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// for passing data from vertex to fragment (v-2-f)
struct v2f_t
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: UV;

	float3 world_pos		: WORLD_POSITION;
	float3 world_tangent	: WORLD_TANGENT;
	float3 world_bitangent	: WORLD_BITANGENT;
	float3 world_normal		: WORLD_NORMAL;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction( vs_input_t input )
{
	v2f_t v2f = (v2f_t)0;

	float4 local_pos	= float4( input.position, 1 );
	float4 world_pos	= mul( MODEL, local_pos );
	float4 view_pos		= mul( VIEW, world_pos );
	float4 clip_pos		= mul( PROJECTION, view_pos );// = worldPos; // might have a w

	float4 local_normal = float4( input.normal, 0.0f );
	float4 world_normal = mul( MODEL, local_normal );
	float4 local_tangent = float4( input.tangent, 0.0f );
	float4 world_tangent = mul( MODEL, local_tangent );
	float4 local_bitangent = float4( input.bitangent, 0.0f );
	float4 world_bitangent = mul( MODEL, local_bitangent );

	v2f.pos = clip_pos;
	v2f.color = input.color * TINT;
	v2f.uv = input.uv;
	v2f.world_pos = world_pos.xyz;
	v2f.world_tangent = world_tangent.xyz;
	v2f.world_bitangent = world_bitangent.xyz;
	v2f.world_normal = world_normal.xyz;
	return v2f;
}

// raster step
/*
float3 ndcPos = clipPos.xyz / clipPos.w;

*/
//--------------------------------------------------------------------------------------
// Fragment Shader
//
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
	// TBN
	float3 normal = normalize( input.world_normal );
	float3 tangent = normalize( input.world_tangent );
	float3 bitangent = normalize( input.world_bitangent );

	float3x3 TBN = float3x3( tangent, bitangent, normal );

	float3x3 world_to_surface = GetWorldToSurfaceTransform( tangent, bitangent, normal );
	float3 look_dir = normalize( input.world_pos - CAMERA_POSITION );
	float3 surf_look_dir = mul( look_dir, world_to_surface );

	float2 parallax_uv = ComputeDeepParallaxUV( surf_look_dir, input.uv );
	//return float4( parallax_uv, 0.f, 1.f );

	// object
	float4 texture_color = tDiffuse.Sample( sSampler, parallax_uv );
	float4 texture_normal_color = tNormal.Sample( sSampler, parallax_uv );
	float3 surface_normal = NormalColorToVector( texture_normal_color ); 
	float3 final_normal = mul( surface_normal, TBN );
	float4 object_color = texture_color * input.color;


	// phong model
	// ambient
	float3 final_color = ComputeLightAt( input.world_pos, final_normal, object_color );
	return float4( final_color, object_color.w );
}
