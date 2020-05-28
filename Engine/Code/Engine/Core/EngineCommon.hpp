#pragma once
#include <string>
#include "Engine/Core/DevConsole/DevConsole.hpp"
#include "Engine/Core/Error/ErrorWarningAssert.hpp"
#include "Engine/Core/Event/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Debug/DebugRender.hpp"

#define UNUSED(x) (void)(x);
#define SELF_SAFE_RELEASE(ptr) if(nullptr !=ptr) { delete ptr; ptr = nullptr; } 

typedef unsigned int uint;
typedef unsigned char byte;

extern DevConsole* g_theConsole;
extern EventSystem* g_theEventSystem;
extern BitmapFont* g_defaultFont;
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

const std::string SHADER_LOCATION { "data/Shader/" };
const std::string DEFAULT_SHADER_LOCATION { "data/Shader/default.hlsl" };

constexpr int MAX_LIGHTS_NUM = 8;