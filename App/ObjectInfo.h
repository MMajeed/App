#ifndef __ObjectInfo__
#define __ObjectInfo__

#include "iObjectDrawable.h"

class ObjectInfo
{
public:	
	iObjectDrawable* ObjectDrawable;
	bool DrawNext;
	float Sort;

	ObjectInfo();
	ObjectInfo(const ObjectInfo& obj);
	ObjectInfo& operator=(const ObjectInfo& obj);
	~ObjectInfo();
};


#endif //__ObjectInfo__