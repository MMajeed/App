#ifndef __CarInput__
#define __CarInput__

#include "InputObject.h"
#include <memory>


class CarInput : public InputObject
{
public:
	virtual void Init();
	virtual void Clean();
	virtual void KeyChange(unsigned int key, bool KeyUp);
	virtual void Update(float delta);
	CarInput(ObjectInfo* o);

	bool ShiftStatus;
	bool WKeyStatus;
	bool AKeyStatus;
	bool SKeyStatus;
	bool DKeyStatus;

	virtual ~CarInput();
};


#endif //__CarInput__