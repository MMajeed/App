#ifndef __AnimationState__
#define __AnimationState__

#include "InputObject.h"

class PlayerInput;

class AnimationState
{
public:
	float deltaTime;
	virtual void HandleKey(unsigned int key, bool KeyUp, PlayerInput* input) = 0;
	virtual void Update(float delta, PlayerInput* input) = 0;
	AnimationState() : deltaTime(0.0f){}
};

class IdleAnimantion : public AnimationState
{
public:
	virtual void HandleKey(unsigned int key, bool KeyUp, PlayerInput* input);
	virtual void Update(float delta, PlayerInput* input);
};

class WalkingAnimantion : public AnimationState
{
public:
	virtual void HandleKey(unsigned int key, bool KeyUp, PlayerInput* input);
	virtual void Update(float delta, PlayerInput* input);
};

class IdleToRunAnimantion : public AnimationState
{
public:
	virtual void HandleKey(unsigned int key, bool KeyUp, PlayerInput* input);
	virtual void Update(float delta, PlayerInput* input);
};

class RunAnimantion : public AnimationState
{
public:
	virtual void HandleKey(unsigned int key, bool KeyUp, PlayerInput* input);
	virtual void Update(float delta, PlayerInput* input);
};

class RunToIdleAnimantion : public AnimationState
{
public:
	virtual void HandleKey(unsigned int key, bool KeyUp, PlayerInput* input);
	virtual void Update(float delta, PlayerInput* input);
};

#endif //__AnimationState__