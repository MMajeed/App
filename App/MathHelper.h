#ifndef __MathHelper__
#define __MathHelper__

#include <Windows.h>
#include <xnamath.h>

class MathHelper
{
public:
	static float Length(XMFLOAT4 v1, XMFLOAT4 v2);
	static float Length(XMFLOAT4 v1);
	static XMFLOAT4 Normalize(XMFLOAT4 v1);
	static XMFLOAT4 Identity();
};

XMFLOAT4 operator-(const XMFLOAT4& rh, const XMFLOAT4& lh);
XMFLOAT4 operator+(const XMFLOAT4& rh, const XMFLOAT4& lh);
XMFLOAT4 operator*(const XMFLOAT4& rh, float lh);
XMFLOAT4 operator*(float rh, const XMFLOAT4&lh);
XMFLOAT4 operator/(const XMFLOAT4& rh, float lh);
XMFLOAT4 operator/(float rh, const XMFLOAT4&lh);


#endif //__MathHelper__