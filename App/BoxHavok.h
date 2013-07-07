#ifndef _BoxHavoK_
#define _BoxHavoK_

#include "HavokObject.h"

class BoxHavok : public HavokObject
{
public:
	virtual void Init(ObjectInfo* o);

	hkVector4 halfExtent;
	hkVector4 startingPosition;
	hkQuaternion startingRotation;
	float mass;
	float restitution;
	float friction;
	bool moveable;

	BoxHavok();
	static BoxHavok* Spawn(std::map<std::string, std::string> info);

};

#endif //_BoxHavoK_