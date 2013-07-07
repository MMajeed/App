#include "ObjectInfo.h"
#include "IPhysicsObject.h"

void ObjectInfo::Update(float delta)
{
	if(this->InputObject != 0)
	{
		this->InputObject->Update(delta);
	}

	if(this->ObjectDrawable != 0)
	{		
		if(this->Physics != 0 )
		{
			this->ObjectDrawable->object.Pos = this->Physics->GetPosition();
			this->ObjectDrawable->object.Rot = this->Physics->GetRotation();
			this->ObjectDrawable->object.Rot.w = 0.0f;
		}

		this->ObjectDrawable->UpdateDrawing(delta);
		this->ObjectDrawable->UpdateObject(delta);

		if(this->ObjectDrawable->object.Pos.y < -200.0f)
		{
			this->Delete = true;
		}
	}
}

ObjectInfo::ObjectInfo()
{
	this->InputObject    = NULL;
	this->Physics        = NULL;
	this->ObjectDrawable = NULL;
	this->DrawNext       = true;
	this->Sort           = 0.0f;
	this->Delete		 = false;
}

ObjectInfo::ObjectInfo(const ObjectInfo& obj)
{
	this->InputObject    = NULL;
	this->Physics        = NULL;
	this->ObjectDrawable = NULL;
	this->DrawNext       = obj.DrawNext;
	this->Sort           = obj.Sort;

	if(obj.ObjectDrawable != 0)
	{
		this->ObjectDrawable = obj.ObjectDrawable->clone();
	}
}
ObjectInfo& ObjectInfo::operator=(const ObjectInfo& obj)
{
	this->InputObject    = NULL;
	this->Physics        = NULL;
	this->ObjectDrawable = NULL;
	this->DrawNext       = obj.DrawNext;
	this->Sort           = obj.Sort;

	if(obj.ObjectDrawable != 0)
	{
		this->ObjectDrawable = obj.ObjectDrawable->clone();
	}
	return *this;
}
ObjectInfo::~ObjectInfo()
{
	if(this->ObjectDrawable != 0)
	{
		this->ObjectDrawable->Clean();
		delete (this->ObjectDrawable);
		this->ObjectDrawable = 0;
	}

	if(this->Physics != 0)
	{
		this->Physics->shutDown();
		delete (this->Physics);
		this->Physics = 0;
	}

}