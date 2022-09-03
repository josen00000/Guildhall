#pragma once
struct Vec2 {
public:
	float x = 0.f;
	float y = 0.f;

	static float CrossProduct( Vec2 const& a, Vec2 const& b );
private:

public:
	Vec2() = default;
	~Vec2() = default;
	Vec2( Vec2 const& copyFrom ) = default;
	Vec2( Vec2&& moveFrom ) = default;
	explicit Vec2( IntVec2 const& intVec );
	explicit Vec2( const float i_x, const float i_y );
	explicit Vec2( const float i_xy );

	static Vec2 MakeFromPolarRadians( float directionRadians, float length=1.f );
	static Vec2 MakeFromPolarDegrees( float directionDegrees, float length=1.f );

	// Accessor
	float GetLength() const;
	float GetSquaredLength() const;
	float GetAngleDegrees() const;
	float GetAngleRadians() const;

	const Vec2 GetNormalized() const;

	// Mutator
	void SetRadians( float i_rads );
	void SetDegrees( float i_degs );
	void SetPolarRadians( float i_rads, float i_length );
	void SetPolarDegrees( float i_degs, float i_length );
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void RotateRadians( float deltaRadians );
	void RotateDegrees( float deltaDegrees );
	void SetLength( float i_length );
	void ClampLength( float maxLength, float minLength = 0 );
	void Normalize();


	// Operator
	bool		operator==( const Vec2& compare ) const;		// vec2 == vec2
	bool		operator!=( const Vec2& compare ) const;		// vec2 != vec2
	const Vec2	operator+( const Vec2& vecToAdd ) const;		// vec2 + vec2
	const Vec2	operator-( const Vec2& vecToSubtract ) const;	// vec2 - vec2
	const Vec2	operator-() const;								// -vec2, i.e. "unary negation"
	const Vec2	operator*( float uniformScale ) const;			// vec2 * float
	const Vec2	operator*( const Vec2& vecToMultiply ) const;	// vec2 * vec2
	const Vec2	operator/( float inverseScale ) const;			// vec2 / float
	void		operator+=( const Vec2& vecToAdd );				// vec2 += vec2
	void		operator-=( const Vec2& vecToSubtract );		// vec2 -= vec2
	void		operator*=( const float uniformScale );			// vec2 *= float
	void		operator/=( const float uniformDivisor );		// vec2 /= float
	void		operator=( const Vec2& copyFrom );				// vec2 = vec2
};