#include "common.hlsl"

float3 ComputeAttenuation( float3 atten, float dist ) {
	float3 v3_dist = float3( 1, dist, ( dist * dist ) );
	return dot( atten, v3_dist );
}

float2 ComputeLightFactor( light_t light, float3 world_pos, float3 world_normal, float3 point_to_eye_dir ){
	float light_intensity = ComputeLightIntensity( light, world_pos );
	float light_dist = distance( world_pos, light.world_pos );
	float diffuse_atten = ComputeAttenuation( light.diffuse_attenuation, light_dist );
	float specular_atten = ComputeAttenuation( light.specular_attenuation, light_dist );
	float3 point_to_light_dir = normalize( light.world_pos - world_pos );

	// diffuse
	float diffuse_strength = max( dot( point_to_light_dir, world_normal ), 0.0f );
	float diffuse_intensity = light.color_intensity.w / diffuse_atten;
	float diffuse = diffuse_strength * diffuse_intensity;

	// specular
	// might add facing later
	float half_dir = normalize( ( point_to_light_dir + point_to_eye_dir ) / 2.f );
	float specular_strength = pow( max( dot( half_dir, world_normal ), 0.0f ), SPECULAR_POWER );
	float specular_intensity = light.color_intensity.w / specular_atten;
	float specular = specular_intensity * specular_strength;
	return float2( diffuse, specular );
}

float3 ComputeLightAt( float3 world_pos, float3 world_normal, float3 surf_color ) {
	float3 point_to_eye_dir = normalize( CAMERA_POSITION - world_pos );

	float3 ambient = SCENE_DATA.ambient_light.xyz * SCENE_DATA.ambient_light.w;
	float3 diffuse = float3( 0.f, 0.f, 0.f );
	float3 specular = float3( 0.f, 0.f, 0.f );
	for( int i = 0; i < MAX_LIGHTS; i++ ) {
		light_t temp_light = SCENE_DATA.lights[i];
		float3 light_color = temp_light.color_intensity.xyz;
		float2 light_factors = ComputeLightFactor( temp_light, world_pos, world_normal, point_to_eye_dir );
		diffuse += light_factors.x * temp_light.color_intensity.xyz;
		specular += light_factors.y * temp_light.color_intensity.xyz;
	}

	diffuse = min( diffuse, float3( 1.0f, 1.0f, 1.0f ) );
	specular *= SPECULAR_FACTOR;

	return ( diffuse + ambient + specular ) * surf_color;
}

