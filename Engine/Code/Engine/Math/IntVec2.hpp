#pragma once
struct Vec2;
//-----------------------------------------------------------------------------------------------
struct IntVec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x = 0;
	int y = 0;

	static const IntVec2 ZERO;
	static const IntVec2 ONE;

public:
	// Construction/Destruction
	~IntVec2() {}											// destructor (do nothing)
	IntVec2() {}												// default constructor (do nothing)
	IntVec2( const IntVec2& copyFrom );
	explicit IntVec2( int XY );
	explicit IntVec2(const Vec2 vec2);
	explicit IntVec2(const int initialX,const int initialY );		// explicit constructor (from x, y)
	explicit IntVec2( int XY );
															// Static method(e.g. creation functions)



	// Accessor(const methods)
	float		GetLength() const;
	float		GetLengthSquared() const;
	int			GetTaxicabLength() const;
	float		GetAngleRadians() const;
	float		GetAngleDegrees() const;
	const IntVec2 GetRotated90Degrees() const;
	const IntVec2 GetRotatedMinus90Degrees() const;
	
	
	
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	
	
	
	//const IntVec2 GetClamped( float maxLength ) const;
	//const IntVec2 GetNormalized() const;

	// Mutators(non-const methods)
	void SetFromText( const char* text);
	/*

	void SetAngleRadians( float newAngleRadians );
	void SetAngleDegrees( float newAngleDegrees );
	void SetPolarRadians( float newAngleRadians, float newLength );
	void SetPolarDegrees( float newAngleDegrees, float newLength );
	*/
	//void RotateRadians( float deltaRadians );
	//void RotateDegrees( float deltaDegrees );
	//void SetLength( float newLength );
	//void ClampLength( float maxLength );
	//void Normalize();
	//float NormalizeAndGetPreviousLength();
	

	// Operators (const)
	bool		operator==( const IntVec2& compare ) const;		// vec2 == vec2
	bool		operator!=( const IntVec2& compare ) const;		// vec2 != vec2
	const IntVec2	operator+( const IntVec2& vecToAdd ) const;		// vec2 + vec2
	const IntVec2	operator-( const IntVec2& vecToSubtract ) const;	// vec2 - vec2
	const IntVec2	operator-() const;								// -vec2, i.e. "unary negation"
	const IntVec2	operator*( int uniformScale ) const;			// vec2 * float
	const IntVec2	operator*( const IntVec2& vecToMultiply ) const;	// vec2 * vec2
	//const IntVec2	operator/( float inverseScale ) const;			// vec2 / float

																// Operators (self-mutating / non-const)
	void		operator+=( const IntVec2& vecToAdd );				// vec2 += vec2
	void		operator-=( const IntVec2& vecToSubtract );		// vec2 -= vec2
	void		operator*=( const int uniformScale );			// vec2 *= float
	//void		operator/=( const float uniformDivisor );		// vec2 /= float
	void		operator=( const IntVec2& copyFrom );				// vec2 = vec2

																// Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
	friend const IntVec2 operator*( int uniformScale, const IntVec2& vecToScale );	// float * vec2
};


