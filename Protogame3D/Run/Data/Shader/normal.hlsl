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

struct light_t {
	float3 worldPosition;
	float light_pad;
	float3 color;
	float intensity;
};
static float SHIFT = 0.75f;

cbuffer time_constants : register(b0){
	float SYSTEM_TIME_SECONDS;
	float SYSTEM_TIME_DELTA_SECONDS;
};

// MVP - Model - View - Projections
cbuffer camera_constants : register(b1) {
	float4x4 PROJECTION; // CAMERA_TO_CLIP_TRANSFORM
	float4x4 VIEW;
	float3	CAMERA_POSITION;
	float	camera_pad;
};

cbuffer model_constants : register(b2) {
	float4x4 MODEL;
	float SPECULAR_FACTOR;
	float SPECULAR_POWER;
	float2 model_pad;
}

cbuffer tint_constants : register(b3) {
	float4 TINT;
}

cbuffer light_constants : register(b4) {
	light_t LIGHT;
}

cbuffer ambient_constants : register(b5) {
	float4 AMBIENT_LIGHT;
}

Texture2D <float4> tDiffuse	: register(t0);
SamplerState sSampler: register(s0);
//--------------------------------------------------------------------------------------
// Programmable Shader Stages
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// for passing data from vertex to fragment (v-2-f)
struct v2f_t 
{
	float4 position : SV_POSITION; 
	float4 color : COLOR; 
	float2 uv : UV; 

	float3 worldPosition : WORLD_POSITION;
	float3 worldNormal : WORLD_NORMAL;
}; 

float RangeMap( float val, float inMin, float inMax, float outMin, float outMax ){
	float domain = inMax - inMin;
	float range = outMax - outMin;
	return( (val - inMin) / domain ) * range + outMin;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction( vs_input_t input )
{
	v2f_t v2f = (v2f_t)0;

	// forward vertex input onto the next stage

	float4 localPos		= float4( input.position, 1 );
	float4 worldPos		= mul( MODEL, localPos );
	float4 cameraPos	= mul(VIEW, worldPos );
	float4 clipPos		= mul( PROJECTION, cameraPos );// = worldPos; // might have a w

	float4 localNormal = float4( input.normal, 0.0f );
	float4 worldNormal = mul( MODEL, localNormal );

	v2f.position = clipPos;
	v2f.color = input.color * TINT; 
	v2f.uv = input.uv; 
	v2f.worldPosition = worldPos.xyz;
	v2f.worldNormal = worldNormal.xyz;
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

	float3 surfaceNormal = normalize( input.worldNormal );
	float3 color = ( surfaceNormal + float3( 1.0f, 1.0f, 1.0f ) ) * 0.5f;

	
	return float4( color, 1.f );
}