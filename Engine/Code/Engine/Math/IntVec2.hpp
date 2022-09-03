#pragma once

struct Vec2;

struct IntVec2 {
	int x = 0;
	int y = 0;

public:
	IntVec2() = default;
	~IntVec2() = default;
	explicit IntVec2( int xy );
	explicit IntVec2( int i_x, int i_y );
	explicit IntVec2( Vec2 const& vec );

	// Mutator
	float			GetLength() const;
	float			GetLengthSquared() const;
	int				GetTaxicabLength() const;
	float			GetAngleRadians() const;
	float			GetAngleDegrees() const;
	const IntVec2	GetRotated90Degrees() const;
	const IntVec2	GetRotatedMinus90Degrees() const;
	static float CrossProduct( IntVec2 const& a, IntVec2 const& b );

	// Operators (const)
	bool		operator==( const IntVec2& compare ) const;		// vec2 == vec2
	bool		operator!=( const IntVec2& compare ) const;		// vec2 != vec2
	const IntVec2	operator+( const IntVec2& vecToAdd ) const;		// vec2 + vec2
	const IntVec2	operator-( const IntVec2& vecToSubtract ) const;	// vec2 - vec2
	const IntVec2	operator-() const;								// -vec2, i.e. "unary negation"
	const IntVec2	operator*( int uniformScale ) const;			// vec2 * float
	const IntVec2	operator*( const IntVec2& vecToMultiply ) const;	// vec2 * vec2
	void		operator+=( const IntVec2& vecToAdd );				// vec2 += vec2
	void		operator-=( const IntVec2& vecToSubtract );		// vec2 -= vec2
	void		operator*=( const int uniformScale );			// vec2 *= float
	void		operator=( const IntVec2& copyFrom );				// vec2 = vec2
};
