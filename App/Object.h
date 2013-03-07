#ifndef __Object__
#define __Object__

#include <windows.h>
#include <xnamath.h>
#include "App.h"
#include "MathHelper.h"

class Object
{
public:	
	XMFLOAT4 Pos;
	XMFLOAT4 Rot;
	XMFLOAT4 Scale;	
	XMFLOAT4 Orbit;	

	struct
	{
		XMFLOAT4 Diffuse;
		XMFLOAT4 Ambient;
		XMFLOAT4 Spec;
	} Colour;

	XMFLOAT4 Min; 
	XMFLOAT4 Max;

	XMFLOAT4 GetMax()
	{
		XMFLOAT4 max = Max;
		max.x *= Scale.x;
		max.x += Pos.x;
		max.y *= Scale.y;
		max.y += Pos.y;
		max.z *= Scale.z;
		max.z += Pos.z;
		return max;
	}
	XMFLOAT4 GetMin()
	{
		XMFLOAT4 min = Min;
		min.x *= Scale.x;
		min.x += Pos.x;
		min.y *= Scale.y;
		min.y += Pos.y;
		min.z *= Scale.z;
		min.z += Pos.z;
		return min;
	}

	void Update(float update);
	XMFLOAT4X4 CalculateMatrix() const;

	Object();
};


#endif //__Object__