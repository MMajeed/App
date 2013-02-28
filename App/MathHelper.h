#ifndef __MathHelper__
#define __MathHelper__

#include <Windows.h>
#include <xnamath.h>

class MathHelper
{
public:
	static float Length(XMFLOAT4 v1, XMFLOAT4 v2);
	static XMFLOAT4 Normalize(XMFLOAT4 v1);
};

#endif //__MathHelper__