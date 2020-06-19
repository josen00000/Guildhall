//-----------------------------------------------------------------------------------------------
// Math utility functions (included utility shader)
//
// Designed to be included at the start of any GLSL #version 120 shader.
// #version 120
//


//-----------------------------------------------------------------------------------------------
float SmoothStep( float t )
{
	return t * t * (3.0 - (2.0 * t));
}


//-----------------------------------------------------------------------------------------------
float CalcLuminance( vec4 rgba )
{
	return (rgba.r * .21) + (rgba.g * .71) + (rgba.b * .07);
}
