#include "PlayerInput.h"
#include "AnimationState.h"
#include <Windows.h>

void PlayerInput::Init()
{

}
void PlayerInput::Clean()
{

}
void PlayerInput::KeyChange(unsigned int key, bool KeyUp)
{
	if(key == VK_SHIFT)
	{
		this->ShiftStatus = KeyUp;
	}
	
	if(this->state)
	{
		this->state->HandleKey(key, KeyUp, this);
	}
}

void PlayerInput::Update(float delta)
{
	if(this->state)
	{
		this->state->Update(delta, this);
	}
}

PlayerInput::PlayerInput(ObjectInfo* o)
	: InputObject(o)
{
	this->ShiftStatus = false;
	this->state = std::shared_ptr<AnimationState>(new IdleAnimantion);
}

PlayerInput::~PlayerInput()
{

}