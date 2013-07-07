#ifndef _IPHysicsObject_HG_
#define _IPHysicsObject_HG_

#include "ObjectInfo.h"
#include <Windows.h>
#include <xnamath.h>


class IPhysicsObject
{
public:
	virtual void Init(ObjectInfo* o) = 0;	
	virtual void shutDown()          = 0;

	virtual void ApplyForce(XMFLOAT4 force, float time) = 0;

	virtual XMFLOAT4 GetPosition()   = 0;
	virtual XMFLOAT4 GetRotation()   = 0;
};

#endif //_IPHysicsObject_HG_