#ifndef __Object__
#define __Object__

#include <windows.h>
#include <xnamath.h>

class Object
{
public:	
	XMFLOAT4 Pos;
	XMFLOAT4 Rot;
	XMFLOAT4 Scale;	
	XMFLOAT4 Orbit;	

	void Update(float update);
	XMFLOAT4X4 CalculateMatrix() const;

	Object();
};


#endif //__Object__