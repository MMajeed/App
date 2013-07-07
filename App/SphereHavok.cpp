#include "SphereHavok.h"
#include "HavokPhysics.h"
#include "Helper.h"

void SphereHavok::Init(ObjectInfo* o)
{
	
	hkRefPtr<hkpSphereShape> sphereShape = new hkpSphereShape(this->radius);

	hkpRigidBodyCinfo bodyCinfo;
	bodyCinfo.m_shape = sphereShape;
	bodyCinfo.m_solverDeactivation = bodyCinfo.SOLVER_DEACTIVATION_MEDIUM;
	// Place box somewhere above floor...
	bodyCinfo.m_position = this->startingPosition;
	bodyCinfo.m_rotation = hkQuaternion(this->startingRotation);
	bodyCinfo.m_restitution = this->restitution;
	bodyCinfo.m_friction = this->friction;
	if(moveable)
	{
		//  Calculate the mass properties for the shape
		const hkReal sphereMass = mass;
		//hkpMassProperties massProperties;		// Deprecated for 2012 (was in 2011)
		hkMassProperties massProperties;		// new for 2012
		hkpInertiaTensorComputer::computeShapeVolumeMassProperties(sphereShape, sphereMass, massProperties);

		bodyCinfo.setMassProperties(massProperties);
	}
	else
	{
		bodyCinfo.m_motionType = hkpMotion::MOTION_FIXED;
	}
	// Create the rigid body
	this->m_RigidBody = new hkpRigidBody(bodyCinfo);
	
	this->AddBodyToHavok();
}

SphereHavok::SphereHavok()
{
	this->radius = 0.0f;
	this->startingPosition = hkVector4(0.0f, 0.0f, 0.0f);
	this->mass = 0.0f;
	this->friction = 0.0f;
	this->restitution = 0.0f;
	this->moveable = true;
}

SphereHavok* SphereHavok::Spawn(std::map<std::string, std::string> info)
{
	SphereHavok* newSphere = new SphereHavok();

	// Radius
	auto iter = info.find("Radius");
	if(iter != info.end()) { newSphere->radius = Helper::StringToFloat(iter->second); } 
	
	if(info.find("Scale") != info.end())
	{
		// XYZHalfExtent
		iter = info.find("XYZScaleX");
		if(iter != info.end()) { newSphere->radius *= Helper::StringToFloat(iter->second); }
	}

	// XYZLocation
	iter = info.find("XYZLocationX");
	if(iter != info.end()) { newSphere->startingPosition.setComponent(0, Helper::StringToFloat(iter->second)); } 	
	iter = info.find("XYZLocationY");
	if(iter != info.end()) { newSphere->startingPosition.setComponent(1, Helper::StringToFloat(iter->second)); } 	
	iter = info.find("XYZLocationZ");
	if(iter != info.end()) { newSphere->startingPosition.setComponent(2, Helper::StringToFloat(iter->second)); } 	
	
	float roll = 0; float yaw = 0; float pitch = 0;

	// XYZRotation
	iter = info.find("XYZRotationX");
	if(iter != info.end()) { pitch = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZRotationY");
	if(iter != info.end()) { yaw = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZRotationZ");
	if(iter != info.end()) { roll = Helper::StringToFloat(iter->second); } 	

	newSphere->startingRotation.setFromEulerAngles(roll, pitch, yaw);

	// mass
	iter = info.find("Mass");
	if(iter != info.end()) { newSphere->mass = Helper::StringToFloat(iter->second); } 	

	// restitution
	iter = info.find("Restitution");
	if(iter != info.end()) { newSphere->restitution = Helper::StringToFloat(iter->second); } 	

	// Friction
	iter = info.find("Friction");
	if(iter != info.end()) { newSphere->friction = Helper::StringToFloat(iter->second); } 	
	
	// Moveable
	iter = info.find("Moveable");
	if(iter != info.end()) { newSphere->moveable = Helper::ToLower(iter->second) == "true"; } 	

	return newSphere;
}