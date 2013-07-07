#include "CarInput.h"	
#include "ObjectInfo.h"
#include "IPhysicsObject.h"
#include "HavokObject.h"
#include <windows.h>
#include <xnamath.h>

void CarInput::Init()
{

}

void CarInput::Clean()
{

}

void CarInput::KeyChange(unsigned int key, bool KeyUp)
{
	switch(key)
	{
	case 'W': case 'w':
		this->WKeyStatus = KeyUp;
		break;
	case 'S': case 's':
		this->SKeyStatus = KeyUp;
		break;
	case 'A': case 'a':
		this->AKeyStatus = KeyUp;
		break;
	case 'D': case 'd':
		this->DKeyStatus = KeyUp;
		break;
	}
}

void CarInput::Update(float delta)
{
	XMFLOAT4 pos = this->object->Physics->GetPosition();

	if(this->WKeyStatus == true)
	{
		pos.z += 1.0f * delta;
	}
	if(this->SKeyStatus == true)
	{
		pos.z -= 1.0f * delta;
	}
	if(this->AKeyStatus == true)
	{
		pos.x -= 1.0f * delta;
	}
	if(this->DKeyStatus == true)
	{
		pos.x += 1.0f * delta;
	}

	dynamic_cast<HavokObject*>(this->object->Physics)->m_RigidBody->setPosition(hkVector4(pos.x, pos.y, pos.z));
	
}

CarInput::CarInput(ObjectInfo* o)
	:InputObject(o)
{

}

CarInput::~CarInput()
{

}