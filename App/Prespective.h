#ifndef __Prespective__
#define __Prespective__

#include <windows.h>
#include <xnamath.h>

class Prespective
{
public:
	XMFLOAT4X4 GetPrespective();

	float GetFovAngle();
	float GetWidth();
	float GetHeight();
	float GetMinViewable();
	float GetMaxViewable();

	void SetFovAngle(float value);
	void SetWidth(float value);
	void SetHeight(float value);
	void SetMinViewable(float value);
	void SetMaxViewable(float value);

	Prespective();

protected:	
	float FovAngle;
	float Width;
	float Height;
	float MinViewable;
	float MaxViewable;
};

#endif // __Prespective__