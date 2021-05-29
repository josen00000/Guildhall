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


//-----------------------------------------------------------------------------------------------
void main()
{
	vec4 diffuseTexel = texture2D( u_diffuseTexture, v_texCoords );
	vec4 shadedTexel = diffuseTexel * v_surfaceColor;
	gl_FragData[0] = shadedTexel;
	gl_FragData[1] = vec4( v_texCoords.x, v_texCoords.y, 0.0, 1.0 );
}

