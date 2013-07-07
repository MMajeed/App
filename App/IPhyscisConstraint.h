#ifndef _PhysicsConstraint_
#define _PhysicsConstraint_

#include <string>

class IPhysicsObject;

class IPhysicsConstraint
{
public:
	virtual void AddConstraint(IPhysicsObject* o1, IPhysicsObject* o2) = 0;
	std::string ID;
};

#endif //_PhysicsConstraint_