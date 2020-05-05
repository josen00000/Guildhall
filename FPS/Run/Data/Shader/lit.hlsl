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

struct fragment_output_t {
	float4 color : SV_Target0;
	float4 bloom : SV_Target1;
	float4 normal : SV_Target2;
	float4 albedo : SV_Target3;
	float4 tangent : SV_Target4;
};

Texture2D <float4> tDiffuse		: register(t0);
Texture2D <float4> tNormal		: register(t4);
Texture2D <float4> tDissolve	: register(t8);
SamplerState sSampler: register(s0);

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
//--------------------------------------------------------------------------------------
// Fragment Shader
//
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
fragment_output_t FragmentFunction( v2f_t input ) 
{
	// TBN
	float3 normal = normalize( input.world_normal );
	float3 tangent = normalize( input.world_tangent );
	float3 bitangent = normalize( input.world_bitangent );

	float3x3 TBN = float3x3( tangent, bitangent, normal );

	// object
	float4 texture_color = tDiffuse.Sample( sSampler, input.uv );
	//return float4( VectorToNormalColor( normal ), 1.0f );
	float4 texture_normal_color = tNormal.Sample( sSampler, input.uv );
	float3 surface_normal = NormalColorToVector( texture_normal_color ); 
	float3 final_normal = mul( surface_normal, TBN );
	float4 object_color = texture_color * input.color;

	// phong model
	// ambient
	float3 ambient = SCENE_DATA.ambient_light.xyz * SCENE_DATA.ambient_light.w;
	float3 final_color = ComputeLightAt( input.world_pos, final_normal, object_color );
	final_color += ambient;
	final_color = ApplyFog( input.world_pos, final_color );
	//return float4( final_color, object_color.w );

	float3 bloom = ComputeBloomAt( input.world_pos, final_normal, object_color );

	fragment_output_t output;
	output.color = float4( final_color.xyz, object_color.w  );
	output.bloom = float4( bloom,  1 );
	output.normal = float4( ( final_normal + float3( 1, 1, 1 ) ) * .5f, 1 );
	output.tangent = float4( ( tangent + float3( 1, 1, 1 ) ) * .5f, 1 );

	return output;
}
