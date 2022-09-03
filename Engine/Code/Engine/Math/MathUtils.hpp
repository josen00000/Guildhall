#pragma once

#define _USE_MATH_DEFINES 
#ifndef M_PI
#define  M_PI  3.1415926f 
#endif

struct Vec2;

float ConvertRadiansToDegrees( const float rads );
float ConvertDegreesToRadians( const float degs );
float ClampFloat( float input, float max, float min);
float RangeMapFloat( float inStart, float inEnd, float outStart, float outEnd, float inValue );


bool IsVec2MostEqual( Vec2 const& a, Vec2 const& b, float epsilon = 0.01f );
bool isFloatMostEqual( float a, float b, float epsilon = 0.01f );
