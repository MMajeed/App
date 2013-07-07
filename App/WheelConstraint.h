#ifndef _WheelConstraint_
#define _WheelConstraint_

#include "HavokConstraint.h"
#include "HavokObject.h"

class WheelConstraint : public HavokConstraint
{
public:
	virtual void AddConstraint(IPhysicsObject* o1, IPhysicsObject* o2);
	WheelConstraint();
	hkRefPtr<hkpWheelConstraintData> m_wheelConstraint;
	static WheelConstraint* Spawn(std::map<std::string, std::string> info);
};

#endif //_WheelConstraint_