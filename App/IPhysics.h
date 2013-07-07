#ifndef _IPhysics_HG_
#define _IPhysics_HG_

#include "IPhyscisConstraint.h"
#include <map>
#include <memory>


class IPhysics
{
public:
	virtual void Init(void) = 0;	
	virtual void CleanUp(void) = 0;
	virtual void Update(float delta) = 0;

	std::map<std::string, IPhysicsConstraint*> ConstraintList;

	IPhysics* IPhysics::getInstance();
protected:
	static IPhysics* physicsInstance;
};

#endif