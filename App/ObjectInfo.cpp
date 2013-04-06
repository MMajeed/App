#include "ObjectInfo.h"

ObjectInfo::ObjectInfo()
{
	this->ObjectDrawable = NULL;
	this->DrawNext = true;
	this->Sort = 100000;
}

ObjectInfo::ObjectInfo(const ObjectInfo& obj)
{
	this->ObjectDrawable = NULL;
	this->DrawNext = obj.DrawNext;
	this->Sort = obj.Sort;

	if(obj.ObjectDrawable != 0)
	{
		this->ObjectDrawable = obj.ObjectDrawable->clone();
	}
}
ObjectInfo& ObjectInfo::operator=(const ObjectInfo& obj)
{
	this->ObjectDrawable = NULL;
	this->DrawNext = obj.DrawNext;
	this->Sort = obj.Sort;

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
		delete (this->ObjectDrawable);
	}
}