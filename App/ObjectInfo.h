#ifndef __ObjectInfo__
#define __ObjectInfo__

#include "iObjectDrawable.h"
#include "InputObject.h"

class IPhysicsObject;

class ObjectInfo
{
public:	
	iObjectDrawable*	ObjectDrawable;
	InputObject*		InputObject;
	IPhysicsObject*		Physics;

	bool DrawNext;
	float Sort;
	bool Delete;

	void Update(float delta);

	ObjectInfo();
	ObjectInfo(const ObjectInfo& obj);
	ObjectInfo& operator=(const ObjectInfo& obj);
	~ObjectInfo();
};


#endif //__ObjectInfo__