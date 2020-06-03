#pragma once

struct Vec3;
struct Vec4;

struct Rgba8 {
public:
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;
	

public:
	Rgba8(){}
	~Rgba8(){}
	explicit Rgba8(unsigned char ini_r, unsigned char ini_g, unsigned char ini_b, unsigned char ini_a = 255);
	static Rgba8 GetLerpColor( Rgba8 a, Rgba8 b, float value );
	static Rgba8 GetColorFromVec4( Vec4 input );

	// Accessor
	Vec3 GetVec3Color() const;

	//Mutators
	void SetFromText(const char* text);

	// Operator
	bool operator==( const Rgba8 compareWith ) const;

public:
	static Rgba8 WHITE;
	static Rgba8 BLACK;
	static Rgba8 RED;
	static Rgba8 HALFRED;
	static Rgba8 GREEN;
	static Rgba8 HALFGREEN;
	static Rgba8 BLUE;
	static Rgba8 HALFBLUE;
	static Rgba8 YELLOW;
	static Rgba8 GRAY;
	static Rgba8 DARK_GRAY;
	//static Rgba8 RED;

	
};