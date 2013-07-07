#include "AnimationState.h"
#include "PlayerInput.h"
#include "ObjectInfo.h"
#include "FBXObject.h"
#include <Windows.h>

void IdleAnimantion::HandleKey(unsigned int key, bool KeyUp, PlayerInput* input)
{
	UNREFERENCED_PARAMETER(KeyUp);
	std::shared_ptr<AnimationState> SPas = input->state;

	if(key == 'W' && input->ShiftStatus == false)
	{
		FBXObject* fbx = dynamic_cast<FBXObject*>(input->object->ObjectDrawable);
		if(fbx != 0)
		{
			fbx->PlayAnimation("Walk", AnimationController::CrossFade);
		}
		input->state = std::shared_ptr<AnimationState>(new WalkingAnimantion);
	}
	else if(key == 'W' && input->ShiftStatus == true)
	{
		FBXObject* fbx = dynamic_cast<FBXObject*>(input->object->ObjectDrawable);
		if(fbx != 0)
		{
			fbx->PlayAnimation("StartRun", AnimationController::CrossFade);
		}
		input->state = std::shared_ptr<AnimationState>(new IdleToRunAnimantion);
	}
}
void IdleAnimantion::Update(float delta, PlayerInput* input)
{
	std::shared_ptr<AnimationState> SPas = input->state;

	this->deltaTime += delta;
}

void WalkingAnimantion::HandleKey(unsigned int key, bool KeyUp, PlayerInput* input)
{
	std::shared_ptr<AnimationState> SPas = input->state;

	if(key == 'W' && KeyUp == false)
	{
		FBXObject* fbx = dynamic_cast<FBXObject*>(input->object->ObjectDrawable);
		if(fbx != 0)
		{
			fbx->PlayAnimation("Idle", AnimationController::CrossFade);
		}

		input->state = std::shared_ptr<AnimationState>(new IdleAnimantion);
	}
	else if(key == VK_SHIFT && KeyUp == true)
	{
		FBXObject* fbx = dynamic_cast<FBXObject*>(input->object->ObjectDrawable);
		if(fbx != 0)
		{
			fbx->PlayAnimation("Run", AnimationController::TimeSync);
		}

		input->state = std::shared_ptr<AnimationState>(new RunAnimantion);
	}
}
void WalkingAnimantion::Update(float delta, PlayerInput* input)
{
	std::shared_ptr<AnimationState> SPas = input->state;

	this->deltaTime += delta;
}

void IdleToRunAnimantion::HandleKey(unsigned int key, bool KeyUp, PlayerInput* input)
{
	std::shared_ptr<AnimationState> SPas = input->state;

	if(key == 'W' && KeyUp == false)
	{
		FBXObject* fbx = dynamic_cast<FBXObject*>(input->object->ObjectDrawable);
		if(fbx != 0)
		{
			fbx->PlayAnimation("Idle", AnimationController::CrossFade);
		}

		input->state = std::shared_ptr<AnimationState>(new IdleAnimantion);
	}
}
void IdleToRunAnimantion::Update(float delta, PlayerInput* input)
{
	std::shared_ptr<AnimationState> SPas = input->state;

	this->deltaTime += delta;

	FBXObject* fbx = dynamic_cast<FBXObject*>(input->object->ObjectDrawable);
	if(fbx != 0)
	{
		if(fbx->GetCurrentPhase() > 0.99f)
		{
			fbx->PlayAnimation("Run", AnimationController::SnapShot, 0.2f);

			input->state = std::shared_ptr<AnimationState>(new RunAnimantion);
		}
	}
}

void RunAnimantion::HandleKey(unsigned int key, bool KeyUp, PlayerInput* input)
{
	std::shared_ptr<AnimationState> SPas = input->state;

	if(key == VK_SHIFT && KeyUp == false)
	{
		FBXObject* fbx = dynamic_cast<FBXObject*>(input->object->ObjectDrawable);
		if(fbx != 0)
		{
			fbx->PlayAnimation("Walk", AnimationController::TimeSync);
		}

		input->state = std::shared_ptr<AnimationState>(new WalkingAnimantion);
	}
	else if(key == 'W' && KeyUp == false)
	{
		FBXObject* fbx = dynamic_cast<FBXObject*>(input->object->ObjectDrawable);
		if(fbx != 0)
		{
			fbx->PlayAnimation("StartEnd", AnimationController::CrossFade);
		}

		input->state = std::shared_ptr<AnimationState>(new RunToIdleAnimantion);
	}
}
void RunAnimantion::Update(float delta, PlayerInput* input)
{
	std::shared_ptr<AnimationState> SPas = input->state;

	this->deltaTime += delta;
}

void RunToIdleAnimantion::HandleKey(unsigned int key, bool KeyUp, PlayerInput* input)
{
	UNREFERENCED_PARAMETER(key);
	UNREFERENCED_PARAMETER(KeyUp);
	std::shared_ptr<AnimationState> SPas = input->state;
}
void RunToIdleAnimantion::Update(float delta, PlayerInput* input)
{
	std::shared_ptr<AnimationState> SPas = input->state;

	this->deltaTime += delta;

	FBXObject* fbx = dynamic_cast<FBXObject*>(input->object->ObjectDrawable);
	if(fbx != 0)
	{
		if(fbx->GetCurrentPhase() > 0.99f)
		{
			fbx->PlayAnimation("Idle", AnimationController::SnapShot, 0.5f);

			input->state = std::shared_ptr<AnimationState>(new IdleAnimantion);
		}
	}
}