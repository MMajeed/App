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
	ObjectInfo(iObjectDrawable* object);
};


#endif //__ObjectInfo__