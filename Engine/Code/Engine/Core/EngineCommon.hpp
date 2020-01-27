#pragma once
//Engine code that almost every need to see.
#include "Rgba8.hpp"
#include "Vertex_PCU.hpp"
#include "ErrorWarningAssert.hpp"
#include "StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"


#define UNUSED(x) (void)(x);


extern DevConsole* g_theConsole;
extern NamedStrings g_gameConfigBlackboard;

const Vec2 ALIGN_BOTTOM_LEFT	= Vec2( 0.f, 0.f);
const Vec2 ALIGN_BOTTOM_CENTER	= Vec2( 0.5f, 0.f);
const Vec2 ALIGN_BOTTOM_RIGHT	= Vec2( 1.f, 0.f);

const Vec2 ALIGN_CENTERED_LEFT	= Vec2( 0.f, 0.5f);
const Vec2 ALIGN_CENTERED		= Vec2( 0.5f, 0.5f);
const Vec2 ALIGN_CENTER_RIGHT	= Vec2( 1.f, 0.5f);

const Vec2 ALIGN_TOP_LEFT		= Vec2( 0.f, 1.f );
const Vec2 ALIGN_TOP_CENTER		= Vec2( 0.5f, 1.f );
const Vec2 ALIGN_TOP_RIGHT		= Vec2( 1.f, 1.f);
