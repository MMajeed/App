#ifndef _HavokConstraint_
#define _HavokConstraint_

#include "IPhyscisConstraint.h"
#include "HavokObject.h"

class HavokConstraint : public IPhysicsConstraint
{
public:
	virtual void AddConstraint(IPhysicsObject* o1, IPhysicsObject* o2) = 0;
	hkRefPtr<hkpConstraintInstance> m_hkConstraint;
};

#endif //_HavokConstraint_