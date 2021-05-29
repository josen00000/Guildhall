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

Texture2D <float4> tDiffuse			: register(t0);
Texture2D <float4> tDiffuse_x		: register(t1);
Texture2D <float4> tDiffuse_y		: register(t2);
Texture2D <float4> tDiffuse_z		: register(t3);
Texture2D <float4> tNormal			: register(t4);
Texture2D <float4> tNormal_x		: register(t5);
Texture2D <float4> tNormal_y		: register(t6);
Texture2D <float4> tNormal_z		: register(t7);
Texture2D <float4> tDissolve		: register(t8);
SamplerState sSampler: register(s0);

float3 ApplyFog( float3 pos, float3 camera_pos, float3 color ){
	float dist = distance( camera_pos, pos );
	float fog_amount = lerp( SCENE_DATA.fog_near_dist, SCENE_DATA.fog_far_dist, dist );
	float3 fog_color = lerp( SCENE_DATA.fog_near_color, SCENE_DATA.fog_far_color, float3( fog_amount, fog_amount, fog_amount ) );
	return fog_color + color;
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
	// triplanar
	float2 x_uv = frac( input.worldPosition.zy ); // minusz, y
	float2 y_uv = frac( input.worldPosition.xz ); // x, minus z
	float2 z_uv = frac( input.worldPosition.xy ); // 

	//x_uv.y = 1 - x_uv.y;
	x_uv.x = 1 - x_uv.x;
	y_uv.y = 1 - y_uv.y;
	
	float4 x_color = tDiffuse_x.Sample( sSampler, x_uv );
	float4 y_color = tDiffuse_y.Sample( sSampler, y_uv );
	float4 z_color = tDiffuse_z.Sample( sSampler, z_uv );
	//float4 test_color = x_color + y_color + z_color;
	//return float4( test_color.xyz, 1 );

	float3 x_normal = tNormal_x.Sample( sSampler, x_uv ).xyz;
	float3 y_normal = tNormal_y.Sample( sSampler, y_uv ).xyz;
	float3 z_normal = tNormal_z.Sample( sSampler, z_uv ).xyz;

	//surfaceNormal = ( textureNormalColor * 2.f - float4( 1.0f, 1.0f, 1.0f, 1.0f ) ).xyz;
	//return float4( x_normal, 1.0f );
	
	x_normal = x_normal * float3( 2.f, 2.0f, 1.0f ) - float3( 1.0f, 1.0f, 0.0f );
	y_normal = y_normal * float3( 2.f, 2.0f, 1.0f ) - float3( 1.0f, 1.0f, 0.0f );
	z_normal = z_normal * float3( 2.f, 2.0f, 1.0f ) - float3( 1.0f, 1.0f, 0.0f );
	z_normal.z= sign(input.worldNormal.z) * z_normal.z;

	float3 x_dir = float3( 1.f, 0.f, 0.f );
	float3 y_dir = float3( 0.f, 1.f, 0.f );
	float3 z_dir = float3( 0.f, 0.f, 1.f );

	float3x3 x_tbn = float3x3( -z_dir, y_dir, x_dir );
	x_normal = mul( x_normal, x_tbn );
	x_normal.x *= sign( input.worldNormal.x );
	//return float4( x_normal, 1.0f );

	float3x3 y_tbn = float3x3( x_dir, -z_dir, y_dir );
	y_normal = mul( y_normal, y_tbn );
	y_normal.y *= sign( input.worldNormal.y );

	float3 weights = normalize(input.worldNormal);
	weights *= weights;


	float4 final_color = weights.x * x_color + weights.y * y_color + weights.z * z_color;


	float3 final_normal = weights.x * x_normal + weights.y * y_normal + weights.z * z_normal;
	final_normal = normalize( final_normal );

	//float4 test_normal_color = float4 ( (final_normal + float3( 1.0f, 1.0f, 1.0f )) * 0.5f, 1.0f );
	//return  test_normal_color;



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
	// test
	finalNormal = final_normal;
	textureColor = final_color;

	float4 objectColor = textureColor * input.color;
	
	// phong model
	// ambient
	float3 ambient = SCENE_DATA.ambientLight.xyz * SCENE_DATA.ambientLight.w;
	float3 cameraPos = CAMERA_POSITION;
	float3 cameraDirection = normalize( cameraPos - input.worldPosition );
	
	// diffuse and specular
	float3 diffuse = float3( 0.f, 0.f, 0.f );
	float3 specular = float3( 0.f, 0.f, 0.f );
	for( int i = 0; i < MAX_LIGHT; i++ ) {
		light_t tempLight = SCENE_DATA.lights[i];
		float tempLightIntensity = CalculateLightIntensity( tempLight, input.worldPosition );
		float3 lightPos = tempLight.worldPosition;
		float lightDist = distance( input.worldPosition, lightPos );
		float3 surfaceToLightDirection = normalize( lightPos - input.worldPosition );
	
	
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
	float3 finalColor = ApplyFog( input.worldPosition, cameraPos, phongColor );
	//float3 phongColor = ( ambient + diffuse ) * objectColor.xyz;
	return float4( phongColor, objectColor.w );
}
