//#include "common.hlsl"
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

const static int MAX_LIGHT = 8;

struct light_t {
	float3 worldPosition;
	float light_pad;
	float4 color_intensity;
	float3 diffuseAttenuation;
	float cosInnerAngle;
	float3 specularAttenuation;
	float cosOuterAngle;
	float3 direction;
	float direction_factor;
};
struct scene_data_t {
	float4 ambientLight;
	float4 emissiveLight;

	light_t lights[MAX_LIGHT];

	float diffuse_factor;
	float emissive_factor;

	float fog_near_dist;
	float fog_far_dist;

	float3 fog_near_color;
	float pad_0;
	float3 fog_far_color;
	float pad_1;
};
static float SHIFT = 0.75f;

float RangeMap( float val, float inMin, float inMax, float outMin, float outMax ){
	float domain = inMax - inMin;
	float range = outMax - outMin;
	return( (val - inMin) / domain ) * range + outMin;
}

float CalculateLightIntensity( light_t light, float3 position ){
	float3 lightToSurfaceDirection = normalize( position - light.worldPosition );
	float cosTheta = dot( lightToSurfaceDirection, light.direction );
	float intensityAmount = RangeMap( cosTheta, light.cosOuterAngle, light.cosInnerAngle, 0.f, light.color_intensity.w );
	intensityAmount = clamp( intensityAmount, 0.f, 1.f );
	intensityAmount = smoothstep( 0.f, 1.f, intensityAmount );
	return intensityAmount;
}

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

cbuffer scene_constants : register(b4) {
	scene_data_t SCENE_DATA;
}

Texture2D <float4> tDiffuse		: register(t0);
Texture2D <float4> tNormal		: register(t1);
Texture2D <float4> tDissolve	: register(t8);
SamplerState sSampler: register(s0);

float3 ApplyFog( float3 pos, float3 camera_pos, float3 color ){
	float dist = distance( pos, camera_pos );
	float3 test = pos - camera_pos;
	float test_dist = sqrt( dot( test, test ) );
	float fog_amount = lerp( SCENE_DATA.fog_near_dist, SCENE_DATA.fog_far_dist, dist );
	float3 fog_color = lerp( SCENE_DATA.fog_near_color, SCENE_DATA.fog_far_color, float3( fog_amount, fog_amount, fog_amount ) );
	return float3( fog_amount.xxx ); 
	//return lerp( color, SCENE_DATA.fog_far_color, fog_amount.xxx );
}

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

	float3 worldPosition	: WORLD_POSITION;
	float3 worldTangent		: WORLD_TANGENT;
	float3 worldBitangent	: WORLD_BITANGENT;
	float3 worldNormal		: WORLD_NORMAL;
};


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
	float4 localTangent = float4( input.tangent, 0.0f );
	float4 worldTangent = mul( MODEL, localTangent );
	float4 localBitangent = float4( input.bitangent, 0.0f );
	float4 worldBitangent = mul( MODEL, localBitangent );

	v2f.position = clipPos;
	v2f.color = input.color * TINT;
	v2f.uv = input.uv;
	v2f.worldPosition = worldPos.xyz;
	v2f.worldTangent = worldTangent.xyz;
	v2f.worldBitangent = worldBitangent.xyz;
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
	// TBN
	float3 normal = normalize( input.worldNormal );
	float3 tangent = normalize( input.worldTangent );
	float3 bitangent = normalize( input.worldBitangent );

	float3x3 TBN = float3x3( tangent, bitangent, normal );

	// object
	float4 textureColor = tDiffuse.Sample( sSampler, input.uv );
	float4 textureNormalColor = tNormal.Sample( sSampler, input.uv );
	float3 surfaceNormal = ( textureNormalColor * 2.f - float4( 1.0f, 1.0f, 1.0f, 1.0f ) ).xyz;
	float3 finalNormal = mul( surfaceNormal, TBN );
	float4 objectColor = textureColor * input.color;

	// phong model
	// ambient
	float3 ambient = SCENE_DATA.ambientLight.xyz * SCENE_DATA.ambientLight.w;
	float3 cameraPos = CAMERA_POSITION;
	float3 cameraDirection = normalize( cameraPos - input.worldPosition );
	float3 test_world_pos = input.worldPosition;

	// diffuse and specular
	float3 diffuse = float3( 0.f, 0.f, 0.f );
	float3 specular = float3( 0.f, 0.f, 0.f );
	for( int i = 0; i < MAX_LIGHT; i++ ) {
		light_t tempLight = SCENE_DATA.lights[i];
		float tempLightIntensity = CalculateLightIntensity( tempLight, input.worldPosition );
		float3 lightPos = tempLight.worldPosition;
		float lightDist = distance( input.worldPosition, lightPos );
		float3 light_to_world = input.worldPosition - lightPos;	
		float planeDist = abs( dot( light_to_world, tempLight.direction ) );
		lightDist = lerp( lightDist, planeDist, tempLight.direction_factor );
		float3 surfaceToLightDirection = normalize( lightPos - input.worldPosition );
		surfaceToLightDirection = lerp( surfaceToLightDirection, -tempLight.direction, tempLight.direction_factor.xxx );

		// diffuse
		float diffuseStrength = max( dot( surfaceToLightDirection, finalNormal ), 0.0f );
		float diffuseIntensity = tempLightIntensity / ( tempLight.diffuseAttenuation.x + tempLight.diffuseAttenuation.y * lightDist + tempLight.diffuseAttenuation.z * pow( lightDist, 2 ) );
		diffuse += diffuseStrength * tempLight.color_intensity.xyz * diffuseIntensity;

		// specular
		float3 halfDirection = normalize( ( cameraDirection + surfaceToLightDirection ) / 2 );
		float specularStrength = max( dot( halfDirection, finalNormal ), 0.0f );
		float expSpecularStrength = pow( specularStrength, SPECULAR_POWER );
		float specularIntensity = tempLightIntensity / ( tempLight.specularAttenuation.x + tempLight.specularAttenuation.y * lightDist + tempLight.specularAttenuation.z * pow( lightDist, 2 ) );
		specular += expSpecularStrength * SPECULAR_FACTOR * tempLight.color_intensity.xyz * specularIntensity;

	}
	diffuse = min( diffuse, float3( 1.0f, 1.0f, 1.0f ) );


	// combine
	float3 phongColor = ( ambient + diffuse + specular ) * objectColor.xyz;
	//float3 finalColor = ApplyFog( input.WorldPosition, CAMERA_POSITION, phongColor );
	//float3 phongColor = ( ambient + diffuse ) * objectColor.xyz;
	// fog
	float camera_dist = distance( input.worldPosition, CAMERA_POSITION );
	//float3 test = pos - camera_pos;
	//float test_dist = sqrt( dot( test, test ) );
	float fog_amount = ( camera_dist - SCENE_DATA.fog_near_dist ) / ( SCENE_DATA.fog_far_dist - SCENE_DATA.fog_near_dist ); 
		//lerp( SCENE_DATA.fog_near_dist, SCENE_DATA.fog_far_dist, camera_dist );
	float3 fog_color = lerp( SCENE_DATA.fog_near_color, SCENE_DATA.fog_far_color, float3( fog_amount, fog_amount, fog_amount ) );
	//return float3( fog_amount.xxx ); 
	float3 final_color = lerp( phongColor, fog_color, fog_amount );

	return float4( final_color, objectColor.w );
}
