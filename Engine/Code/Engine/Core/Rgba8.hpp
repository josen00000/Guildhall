#pragma once
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

	//Mutators
	void SetFromText(const char* text);

public:
	static Rgba8 WHITE;
	static Rgba8 BLACK;
	static Rgba8 RED;
	static Rgba8 GREEN;
	static Rgba8 BLUE;
	static Rgba8 GRAY;
	//static Rgba8 RED;

	
};