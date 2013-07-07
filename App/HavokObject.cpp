#include "HavokObject.h"
#include "HavokPhysics.h"

XMFLOAT4 HavokObject::GetPosition()
{
	if(this->m_RigidBody == NULL && this->m_RigidBody->isActive())
	{
		throw std::exception("Rigid Body is not set but it is trying to accessed in GetPosition");
	}

	XMFLOAT4 returnValue;
	returnValue.x = this->m_RigidBody->getPosition()(0);
	returnValue.y = this->m_RigidBody->getPosition()(1);
	returnValue.z = this->m_RigidBody->getPosition()(2);
	returnValue.w = 1.0f;
	return returnValue;
}

XMFLOAT4 HavokObject::GetRotation()
{
	if(this->m_RigidBody == NULL && this->m_RigidBody->isActive())
	{
		throw std::exception("Rigid Body is not set but it is trying to accessed in GetRotation");
	}

	XMFLOAT4 returnValue;

	hkReal w, x, y, z;
 
    x = this->m_RigidBody->getRotation()(0);
    y = this->m_RigidBody->getRotation()(1);
    z = this->m_RigidBody->getRotation()(2);
    w = this->m_RigidBody->getRotation()(3);
    float sqw = w * w;   
    double sqx = x * x;   
    double sqy = y * y;   
    double sqz = z * z;
   
    returnValue.z = static_cast<float>(atan2(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw)) );
   
    returnValue.x = static_cast<float>(atan2(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw))); 
   
    returnValue.y = static_cast<float>(asin(-2.0 * (x*z - y*w)) );


	return returnValue;
}

void HavokObject::AddBodyToHavok()
{
	// Add the rigidBody to the world	
	HavokPhysics::getInstance()->m_pHavok_world->markForWrite();
	HavokPhysics::getInstance()->m_pHavok_world->addEntity(m_RigidBody);
	HavokPhysics::getInstance()->m_pHavok_world->unmarkForWrite();
}

void HavokObject::ApplyForce(XMFLOAT4 force, float time)
{
	hkVector4 vec4Force(force.x, force.y, force.z, 1.0f);
	this->m_RigidBody->applyForce(time, vec4Force);
}

void HavokObject::shutDown()
{
	if(this->m_RigidBody != NULL && this->m_RigidBody->isActive())
	{
		HavokPhysics::getInstance()->m_pHavok_world->markForWrite();
		HavokPhysics::getInstance()->m_pHavok_world->removeEntity(m_RigidBody);
		HavokPhysics::getInstance()->m_pHavok_world->unmarkForWrite();
	}
}


HavokObject::HavokObject()
{
	this->m_RigidBody = NULL;
}