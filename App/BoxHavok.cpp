#include "BoxHavok.h"
#include "HavokPhysics.h"
#include "Helper.h"

void BoxHavok::Init(ObjectInfo* o)
{
	hkRefPtr<hkpBoxShape> boxShape = new hkpBoxShape(halfExtent);
	
	hkpRigidBodyCinfo bodyCinfo;
	bodyCinfo.m_shape = boxShape;
	bodyCinfo.m_solverDeactivation = bodyCinfo.SOLVER_DEACTIVATION_MEDIUM;
	// Place box somewhere above floor...
	bodyCinfo.m_position = this->startingPosition;
	bodyCinfo.m_rotation = hkQuaternion(this->startingRotation);
	bodyCinfo.m_restitution = this->restitution;
	bodyCinfo.m_friction = this->friction;
	if(moveable)
	{
		//  Calculate the mass properties for the shape
		const hkReal boxMass = this->mass;
		hkMassProperties massProperties;
		hkpInertiaTensorComputer::computeShapeVolumeMassProperties(boxShape, boxMass, massProperties);

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

BoxHavok::BoxHavok()
{
	this->halfExtent = hkVector4(0.0f, 0.0f, 0.0f);
	this->startingPosition = hkVector4(0.0f, 0.0f, 0.0f);
	this->mass = 0.0f;
	this->friction = 0.1f;
	this->restitution = 0.0f;
	this->moveable = true;
}

BoxHavok* BoxHavok::Spawn(std::map<std::string, std::string> info)
{
	BoxHavok* newBox = new BoxHavok();

	// XYZHalfExtent
	auto iter = info.find("XYZHalfExtentX");
	if(iter != info.end()) { newBox->halfExtent.setComponent(0, Helper::StringToFloat(iter->second)); }
	iter = info.find("XYZHalfExtentY");
	if(iter != info.end()) { newBox->halfExtent.setComponent(1, Helper::StringToFloat(iter->second)); }
	iter = info.find("XYZHalfExtentZ");
	if(iter != info.end()) { newBox->halfExtent.setComponent(2, Helper::StringToFloat(iter->second)); }

	if(info.find("Scale") != info.end())
	{
		// XYZHalfExtent
		iter = info.find("XYZScaleX");
		if(iter != info.end()) { newBox->halfExtent.setComponent(0, Helper::StringToFloat(iter->second) * newBox->halfExtent(0) ); }
		iter = info.find("XYZScaleY");
		if(iter != info.end()) { newBox->halfExtent.setComponent(1, Helper::StringToFloat(iter->second) * newBox->halfExtent(1) ); }
		iter = info.find("XYZScaleZ");
		if(iter != info.end()) { newBox->halfExtent.setComponent(2, Helper::StringToFloat(iter->second) * newBox->halfExtent(2) ); }
	}
	
	// XYZLocation
	iter = info.find("XYZLocationX");
	if(iter != info.end()) { newBox->startingPosition.setComponent(0, Helper::StringToFloat(iter->second)); } 	
	iter = info.find("XYZLocationY");
	if(iter != info.end()) { newBox->startingPosition.setComponent(1, Helper::StringToFloat(iter->second)); } 	
	iter = info.find("XYZLocationZ");
	if(iter != info.end()) { newBox->startingPosition.setComponent(2, Helper::StringToFloat(iter->second)); } 	
	
	float roll = 0; float yaw = 0; float pitch = 0;

	// XYZRotation
	iter = info.find("XYZRotationX");
	if(iter != info.end()) { pitch = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZRotationY");
	if(iter != info.end()) { yaw = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZRotationZ");
	if(iter != info.end()) { roll = Helper::StringToFloat(iter->second); } 	

	newBox->startingRotation.setFromEulerAngles(roll, pitch, yaw);

	// mass
	iter = info.find("Mass");
	
	if(iter != info.end()) { newBox->mass = Helper::StringToFloat(iter->second); } 	
	// restitution
	iter = info.find("Restitution");
	if(iter != info.end()) { newBox->restitution = Helper::StringToFloat(iter->second); } 	

	// Friction
	iter = info.find("Friction");
	if(iter != info.end()) { newBox->friction = Helper::StringToFloat(iter->second); } 	
	
	// Moveable
	iter = info.find("Moveable");
	if(iter != info.end()) { newBox->moveable = Helper::ToLower(iter->second) == "true"; } 	

	return newBox;
}
