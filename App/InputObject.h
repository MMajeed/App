#ifndef __InputObject__
#define __InputObject__

#include <string>
#include <vector>

class ObjectInfo;

class InputObject
{
public:
	virtual void Init()					                = 0;
	virtual void Clean()				                = 0;
	virtual void KeyChange(unsigned int key, bool KeyUp)  = 0;
	virtual void Update(float delta)                    = 0;
	ObjectInfo* object;
	InputObject(ObjectInfo* o) : object(o) { }

	virtual ~InputObject(){}
};


#endif //__InputObject__