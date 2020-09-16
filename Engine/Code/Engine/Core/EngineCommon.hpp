#pragma once
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/MathUtils.hpp"

#define UNUSED(x) (void)(x);
#define SELF_SAFE_RELEASE(ptr) if(nullptr !=ptr) { delete ptr; ptr = nullptr; } 
#define BIT_FLAG(b)	(1 << (b))

enum Convention
{
	X_FORWARD_Y_LEFT_Z_UP,
	X_RIGHT_Y_UP_Z_BACKWARD
};

typedef unsigned int uint;
typedef unsigned char byte;

extern DevConsole* g_theConsole;
extern EventSystem* g_theEventSystem;
extern BitmapFont* g_defaultFont;
extern NamedStrings g_gameConfigBlackboard;
extern Convention g_convention;

const Vec2 ALIGN_BOTTOM_LEFT	= Vec2( 0.f, 0.f);
const Vec2 ALIGN_BOTTOM_CENTER	= Vec2( 0.5f, 0.f);
const Vec2 ALIGN_BOTTOM_RIGHT	= Vec2( 1.f, 0.f);

const Vec2 ALIGN_CENTERED_LEFT	= Vec2( 0.f, 0.5f);
const Vec2 ALIGN_CENTERED		= Vec2( 0.5f, 0.5f);
const Vec2 ALIGN_CENTER_RIGHT	= Vec2( 1.f, 0.5f);

const Vec2 ALIGN_TOP_LEFT		= Vec2( 0.f, 1.f );
const Vec2 ALIGN_TOP_CENTER		= Vec2( 0.5f, 1.f );
const Vec2 ALIGN_TOP_RIGHT		= Vec2( 1.f, 1.f);

constexpr int MAX_LIGHTS_NUM = 8;

// network
constexpr int NET_BUFFER_SIZE		= 520;
constexpr int NET_MESSAGE_SIZE		= 512;