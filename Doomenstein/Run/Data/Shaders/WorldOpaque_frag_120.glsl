//-----------------------------------------------------------------------------------------------
// Basic Unlit Textured/Colored Primitive Fragment (Pixel) shader
//
// #version 120
// #include MathUtils
//


//-----------------------------------------------------------------------------------------------
uniform sampler2D	u_diffuseTexture;
uniform float		u_time;


//-----------------------------------------------------------------------------------------------
varying vec4 v_surfaceColor;
varying vec2 v_texCoords;
varying vec3 v_worldPos;


//-----------------------------------------------------------------------------------------------
void main()
{
	vec4 diffuseTexel = texture2D( u_diffuseTexture, v_texCoords );
	vec4 shadedTexel = diffuseTexel * v_surfaceColor;

	if( shadedTexel.r * shadedTexel.g * shadedTexel.b * shadedTexel.a > 0.9 )
	{
		float voob = 0.5 + 0.5 * sin( 2.0 * u_time - 33.0 * v_texCoords.x  + 33.0 * v_texCoords.y );
		voob = 1.0 - 0.5 * (voob * voob * voob * voob * voob * voob * voob * voob);
		shadedTexel.r *= voob;
		shadedTexel.g *= voob;
	}

	gl_FragData[0] = shadedTexel;
	gl_FragData[1] = vec4( v_texCoords.x, v_texCoords.y, 0.0, 1.0 );
}

