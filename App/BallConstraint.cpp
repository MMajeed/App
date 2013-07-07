#include "BallConstraint.h"
#include "HavokPhysics.h"
#include "Application.h"
#include "Helper.h"

void BallConstraint::AddConstraint(IPhysicsObject* o1, IPhysicsObject* o2)
{
	HavokObject* hkO1 = dynamic_cast<HavokObject*>(o1);
	HavokObject* hkO2 = dynamic_cast<HavokObject*>(o2);
	
	hkpRigidBodyCinfo bodyCinfo;
	hkO1->m_RigidBody->getCinfo(bodyCinfo);

	hkpBallAndSocketConstraintData* ballSocket = new hkpBallAndSocketConstraintData();
	hkVector4 pos = bodyCinfo.m_position;
	pos(0) += this->offset(0);	pos(1) += this->offset(1);	pos(2) += this->offset(2);
	ballSocket->setInWorldSpace(hkO1->m_RigidBody->getTransform(), hkO2->m_RigidBody->getTransform(), pos);
	this->m_hkConstraint = new hkpConstraintInstance(hkO1->m_RigidBody, hkO2->m_RigidBody, ballSocket);
	HavokPhysics::getInstance()->AddConstraint(this);	
}

BallConstraint::BallConstraint()
{
	this->offset = hkVector4(0.0f, 0.0f, 0.0, 0.0f);
}

BallConstraint* BallConstraint::Spawn(std::map<std::string, std::string> info)
{
	std::string object1 = info.find("ObjectID1")->second;
	std::string object2 = info.find("ObjectID2")->second;

	if( Application::getInstance()->objects.find(object1) == Application::getInstance()->objects.end()
		|| Application::getInstance()->objects.find(object2) == Application::getInstance()->objects.end() )
	{
		throw std::exception("Constraint objects not found");
	}

	IPhysicsObject* physicObject1 = Application::getInstance()->objects[object1].Physics;
	IPhysicsObject* physicObject2 = Application::getInstance()->objects[object2].Physics;

	BallConstraint* ballConstraint = new BallConstraint();

	// XYZOffsetX
	auto iter = info.find("XYZOffsetX");
	if(iter != info.end()) { ballConstraint->offset.setComponent(0, Helper::StringToFloat(iter->second)); } 	
	iter = info.find("XYZOffsetY");
	if(iter != info.end()) { ballConstraint->offset.setComponent(1, Helper::StringToFloat(iter->second)); } 	
	iter = info.find("XYZOffsetZ");
	if(iter != info.end()) { ballConstraint->offset.setComponent(2, Helper::StringToFloat(iter->second)); } 	
	
	ballConstraint->ID = info.find("ID")->second;

	ballConstraint->AddConstraint(physicObject1, physicObject2);

	return ballConstraint;
}