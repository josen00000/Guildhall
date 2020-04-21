///////////////////////////////////////////
// global variable
///////////////////////////////////////////
const static int MAX_LIGHTS = 8;


///////////////////////////////////////////
// struct
///////////////////////////////////////////
struct light_t {
	float3 world_pos;
	float light_pad0;
	float4 color_intensity;

	float3 diffuse_attenuation;
	float cos_inner_angle;
	float3 specular_attenuation;
	float cos_outer_angle;

	float3 dir;
	float dir_factor;
};

struct scene_data_t {
	float4 ambient_light;
	float4 emissive_light;

	light_t lights[MAX_LIGHTS];

	float diffuse_factor;
	float emissive_factor;
	float2 pad0;

	float fog_near_dist;
	float3 fog_near_color;
	float fog_far_dist;
	float3 fog_far_color;
};


///////////////////////////////////////////
// Uniform Buffer
///////////////////////////////////////////
cbuffer time_constants: register(b0) {
	float SYSTEM_TIME_SECONDS;
	float SYSTEM_TIME_DELTA_SECONDS;
}

cbuffer camera_constants : register(b1) {
	float4x4 PROJECTION;
	float4x4 VIEW;
	float3	CAMERA_POSITION;
	float	CAMERA_PAD0;
}

cbuffer model_constants : register(b2) {
	float4x4 MODEL;
	float SPECULAR_FACTOR;
	float SPECULAR_POWER;
	float2 MODEL_PAD0;
}

cbuffer tint_constants : register(b3) {
	float4 TINT;
}

cbuffer scene_constants : register(b4) {
	scene_data_t SCENE_DATA;
}


///////////////////////////////////////////
// Methods
///////////////////////////////////////////
float RangeMap( float val, float inMin, float inMax, float outMin, float outMax ){
	float domain = inMax - inMin;
	float range = outMax - outMin;
	return( (val - inMin) / domain ) * range + outMin;
}

float ComputeLightToSurfaceDistance( light_t light, float3 pos ) {
	float light_dist = distance( pos, light.world_pos );
	float3 light_to_world = pos - light.world_pos;
	float plane_dist = dot( light_to_world, light.dir );
	return lerp( light_dist, plane_dist, light.dir_factor.x );
}

float3 ComputeLightToSurfaceDirection( light_t light, float3 pos ) {
	float3 light_to_world = normalize( pos - light.world_pos );
	return lerp( light_to_world, light.dir, light.dir_factor.xxx );
}


float3 NormalColorToVector( float3 color ) {
	return  color * 2.f - float3( 1.0f, 1.0f, 1.0f );
}

float3 VectorToNormalColor( float3 vec ) {
	return ( vec + float3( 1.f, 1.f, 1.f ) ) * 0.5f;
}

float ComputeLightIntensity( light_t light, float3 pos ){
	float3 light_to_surface_dir = ComputeLightToSurfaceDirection( light, pos );
	float cos_theta = dot( light_to_surface_dir, light.dir );
	float intensity_amount = RangeMap( cos_theta, light.cos_outer_angle, light.cos_inner_angle, 0.f, light.color_intensity.w );
	intensity_amount = clamp( intensity_amount, 0.f, 1.f );
	intensity_amount = smoothstep( 0.f, 1.f, intensity_amount );
	return intensity_amount;
}

float3x3 GetWorldToSurfaceTransform( float3 tangent, float3 bitangent, float3 normal ) {
	normal = normalize( normal );
	tangent = normalize( tangent );
	bitangent = normalize( bitangent );
	return float3x3( tangent, bitangent, normal );
}

float3x3 GetSurfaceToWorldTransform( float3 tangent, float3 bitangent, float3 normal ) {
	float3x3 surface_to_world = GetWorldToSurfaceTransform( tangent, bitangent, normal );
	return transpose( surface_to_world );
}

float3 ApplyFog( float3 world_pos, float3 color ) {
	float dist = distance( world_pos, CAMERA_POSITION );
	float fog_amount = smoothstep( SCENE_DATA.fog_near_dist, SCENE_DATA.fog_far_dist, dist );
	float3 fog_color = lerp( SCENE_DATA.fog_near_color, SCENE_DATA.fog_far_color, fog_amount.xxx );
	float3 final_color = lerp( color, fog_color, fog_amount );
	return final_color;
}