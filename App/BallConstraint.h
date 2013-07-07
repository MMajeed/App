#ifndef _BallConstraint_
#define _BallConstraint_

#include "HavokConstraint.h"
#include "HavokObject.h"

class BallConstraint : public HavokConstraint
{
public:
	virtual void AddConstraint(IPhysicsObject* o1, IPhysicsObject* o2);
	hkVector4 offset;
	BallConstraint();
	static BallConstraint* Spawn(std::map<std::string, std::string> info);
};

#endif //_BallConstraint_