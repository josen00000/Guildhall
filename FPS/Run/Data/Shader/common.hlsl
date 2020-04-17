const int MAX_LIGHTS = 8;

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
	float3 direction;
	float direction_factor;
};

struct scene_data_t {
	float4 ambient_light;
	float4 emissive_light;

	light_t lights[MAX_LIGHTS];

	float diffuse_factor;
	float emissive_factor;

	float fog_near_distance;
	float fog_far_distance;
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

float CalculateLightIntensity( light_t light, float3 position ){
	float3 lightToSurfaceDirection = normalize( position - light.worldPosition );
	float cosTheta = dot( lightToSurfaceDirection, light.direction );
	float intensityAmount = RangeMap( cosTheta, light.cosOuterAngle, light.cosInnerAngle, 0.f, light.color_intensity.w );
	intensityAmount = clamp( intensityAmount, 0.f, 1.f );
	intensityAmount = smoothstep( 0.f, 1.f, intensityAmount );
	return intensityAmount;
}