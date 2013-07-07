#ifndef _SphereHavok_
#define _SphereHavok_

#include "HavokObject.h"

class SphereHavok : public HavokObject
{
public:
	virtual void Init(ObjectInfo* o);

	hkVector4 startingPosition;
	hkQuaternion startingRotation;
	float radius;
	float mass;
	float restitution;
	float friction;
	bool moveable;

	SphereHavok();
	static SphereHavok* Spawn(std::map<std::string, std::string> info);

};

#endif //_SphereHavok_