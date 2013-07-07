#ifndef __PlayerInput__
#define __PlayerInput__

#include "InputObject.h"
#include <memory>

class AnimationState;

class PlayerInput : public InputObject
{
public:
	virtual void Init();
	virtual void Clean();
	virtual void KeyChange(unsigned int key, bool KeyUp);
	virtual void Update(float delta);
	PlayerInput(ObjectInfo* o);

	bool ShiftStatus;

	std::shared_ptr<AnimationState> state;

	virtual ~PlayerInput();
};


#endif //__PlayerInput__