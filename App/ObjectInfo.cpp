#include "ObjectInfo.h"

ObjectInfo::ObjectInfo()
{
	this->DrawNext = true;
	this->Sort = 100000;
}
ObjectInfo::ObjectInfo(iObjectDrawable* object)
{
	this->ObjectDrawable = object;
	this->DrawNext = true;
	this->Sort = 100000;
}