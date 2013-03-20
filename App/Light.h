#ifndef __Light_HPP__
#define __Light_HPP__

#include <windows.h>
#include <xnamath.h>
#include <string>
#include "cBuffer.h"

class Light
{
public:
	Light();
	virtual ~Light();

	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 spec;

	XMFLOAT4 pos;		
	XMFLOAT4 dir;
	XMFLOAT4 attenuation;	

	enum LightType
	{
		Parallel = 0,
		Point = 1,
		Spot = 2,
		None = 3,
	};

	float power;
	float range;
	LightType type;
};

#endif // __Light_HPP__