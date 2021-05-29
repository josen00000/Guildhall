#pragma once
#include <string>

struct Vec3;
struct IntVec2;

//-----------------------------------------------------------------------------------------------
struct Vec2
{

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;

public:
	// Construction/Destruction
	~Vec2() {}											// destructor (do nothing)
	Vec2() {}												// default constructor (do nothing)
	Vec2( const Vec2& copyFrom );
	explicit Vec2(const IntVec2& copyFrom);
	explicit Vec2( float XY );
	Vec2( const Vec3& copyFrom );	// copy constructor (from another vec2)
	explicit Vec2(const float initialX, const float initialY );		// explicit constructor (from x, y)
		
	// Static method(e.g. creation functions)
	static const Vec2 MakeFromPolarRadians(float directionRadians, float length=1.f );
	static const Vec2 MakeFromPolarDegrees(float directionDegrees, float length=1.f);



	// Accessor(const methods)
	float GetLength() const;
	float GetSquaredLength() const;
	float GetAngleRadians() const;
	float GetAngleDegrees() const;

	const Vec2 GetRotated90Degrees() const;
	const Vec2 GetRotatedMinus90Degrees() const;
	const Vec2 GetRotatedRadians(float deltaRadians) const;
	const Vec2 GetRotatedDegrees(float deltaDegrees) const;
	const Vec2 GetClamped(float maxLength) const;
	const Vec2 GetNormalized() const;
	const Vec2 GetReflected(const Vec2& normal) const;

	std::string ToDebugString();
	std::string ToString();

	// Mutators(non-const methods)
	void SetAngleRadians(float newAngleRadians);
	void SetAngleDegrees(float newAngleDegrees);
	void SetPolarRadians(float newAngleRadians, float newLength);
	void SetPolarDegrees(float newAngleDegrees, float newLength);
	void SetFromText(const char* text);
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void RotateRadians(float deltaRadians);
	void RotateDegrees(float deltaDegrees);
	void SetLength( float newLength);
	void ClampLength(float maxLength);
	void Normalize();
	float NormalizeAndGetPreviousLength();
	void Reflect(const Vec2& normal);

	// static
	static float CrossProduct( Vec2 a, Vec2 b );

	// Operators (const)
	bool		operator==( const Vec2& compare ) const;		// vec2 == vec2
	bool		operator!=( const Vec2& compare ) const;		// vec2 != vec2
	const Vec2	operator+( const Vec2& vecToAdd ) const;		// vec2 + vec2
	const Vec2	operator-( const Vec2& vecToSubtract ) const;	// vec2 - vec2
	const Vec2	operator-() const;								// -vec2, i.e. "unary negation"
	const Vec2	operator*( float uniformScale ) const;			// vec2 * float
	const Vec2	operator*( const Vec2& vecToMultiply ) const;	// vec2 * vec2
	const Vec2	operator/( float inverseScale ) const;			// vec2 / float


	// Operators (self-mutating / non-const)
	void		operator+=( const Vec2& vecToAdd );				// vec2 += vec2
	void		operator-=( const Vec2& vecToSubtract );		// vec2 -= vec2
	void		operator*=( const float uniformScale );			// vec2 *= float
	void		operator/=( const float uniformDivisor );		// vec2 /= float
	void		operator=( const Vec2& copyFrom );				// vec2 = vec2

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
	friend const Vec2 operator*( float uniformScale, const Vec2& vecToScale );	// float * vec2
	
	//static variable
	static Vec2 ZERO;
	static Vec2 ONE;
	static Vec2 ZERO_ONE;
	static Vec2 ONE_ZERO;
	static Vec2 UP_NORMAL_DIRECTION;
	static Vec2 DOWN_NORMAL_DIRECTION;
	static Vec2 LEFT_NORMAL_DIRECTION;
	static Vec2 RIGHT_NORMAL_DIRECTION;
};

