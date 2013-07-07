#ifndef _HavokObject_
#define _HavokObject_

#include "IPhysicsObject.h"
#include "HavokInclude.h"
#include <map>

class HavokObject : public IPhysicsObject
{
public:
	virtual void Init(ObjectInfo* o) = 0;	
	virtual void AddBodyToHavok();
	virtual void shutDown();

	virtual XMFLOAT4 GetPosition();
	virtual XMFLOAT4 GetRotation();

	virtual void ApplyForce(XMFLOAT4 force, float time);

	hkRefPtr<hkpRigidBody> m_RigidBody;

	HavokObject();
};

#endif //_HavokObject_