#ifndef _HingeConstraint_
#define _HingeConstraint_

#include "HavokConstraint.h"
#include "HavokObject.h"

class HingeConstraint : public HavokConstraint
{
public:
	virtual void AddConstraint(IPhysicsObject* o1, IPhysicsObject* o2);
	HingeConstraint();
	float axis;
	static HingeConstraint* Spawn(std::map<std::string, std::string> info);
};

#endif //_HingeConstraint_