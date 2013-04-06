#include "IPhysics.h"
#include <exception>

IPhysics* IPhysics::physicsInstance = 0;

IPhysics* IPhysics::getInstance()
{
	if(IPhysics::physicsInstance == 0)
	{
		throw std::exception("No physics setup");
	}
    return IPhysics::physicsInstance;
}