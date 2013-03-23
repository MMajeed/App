#ifndef __Object__
#define __Object__

#include <windows.h>
#include <xnamath.h>
#include "App.h"
#include "MathHelper.h"

class DrawableInfo
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

	void Update(float update);
	XMFLOAT4X4 CalculateMatrix() const;

	DrawableInfo();
};


#endif //__Object__