//-----------------------------------------------------------------------------------------------
// Basic Unlit Textured/Colored Primitive Vertex shader
//
// #version 120
// #include MathUtils
//

//-----------------------------------------------------------------------------------------------
uniform mat4 u_localToWorldTransform;	// a.k.a. "Model" matrix
uniform mat4 u_worldToEyeTransform;		// a.k.a. "View" matrix
uniform mat4 u_eyeToClipTransform;		// a.k.a. "Projection" matrix


//-----------------------------------------------------------------------------------------------
attribute vec3 a_position;
attribute vec4 a_color;
attribute vec2 a_texCoords;


//-----------------------------------------------------------------------------------------------
varying vec4 v_surfaceColor;
varying vec2 v_texCoords;
varying vec3 v_worldPos;


//-----------------------------------------------------------------------------------------------
void main()
{
	vec4 worldPosition	= u_localToWorldTransform * vec4( a_position, 1.0 );
	vec4 eyePosition	= u_worldToEyeTransform * worldPosition;
	vec4 clipPosition	= u_eyeToClipTransform * eyePosition;

	gl_Position		= clipPosition;
	v_surfaceColor	= a_color;
	v_texCoords		= a_texCoords;
	v_worldPos		= worldPosition.xyz;
}

