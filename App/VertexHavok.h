#ifndef _VertexHavok_
#define _VertexHavok_

#include "HavokObject.h"
#include "PlyBuffer.h"

class VertexHavok : public HavokObject
{
public:
	virtual void Init(ObjectInfo* o);

	hkVector4 startingPosition;
	hkQuaternion startingRotation;
	float mass;
	float restitution;
	float friction;
	bool moveable;
	PlyBuffer PlyBuffer;	

	VertexHavok();
	static VertexHavok* Spawn(std::map<std::string, std::string> info);

};

#endif //_VertexHavok_