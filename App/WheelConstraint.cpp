#include "WheelConstraint.h"
#include "HavokPhysics.h"
#include "Application.h"
#include "Helper.h"

void WheelConstraint::AddConstraint(IPhysicsObject* o1, IPhysicsObject* o2)
{
	HavokObject* hkO1 = dynamic_cast<HavokObject*>(o1);
	HavokObject* hkO2 = dynamic_cast<HavokObject*>(o2);

	//
	// CREATE WHEEL CONSTRAINT
	// 

	this->m_wheelConstraint = new hkpWheelConstraintData();

	hkVector4 suspension(1.0f, 1.0f, 0.0f);
	suspension.normalize3();
	hkVector4 steering(0.0f, 1.0f, 0.0f);
	hkVector4 axle(1.0f, 0.0f, 0.0f);

	m_wheelConstraint->setInWorldSpace( hkO1->m_RigidBody->getTransform(), hkO2->m_RigidBody->getTransform(), 
		hkO1->m_RigidBody->getPosition(), axle, suspension, steering );
	m_wheelConstraint->setSuspensionMaxLimit(0.2f);
	m_wheelConstraint->setSuspensionMinLimit(-0.5f);

	m_wheelConstraint->setSuspensionStrength(0.01f);
	m_wheelConstraint->setSuspensionDamping(HavokPhysics::getInstance()->m_pHavok_world->m_dynamicsStepInfo.m_solverInfo.m_damping * 0.25f);

	this->m_hkConstraint= new hkpConstraintInstance( hkO1->m_RigidBody, hkO2->m_RigidBody, m_wheelConstraint );

	HavokPhysics::getInstance()->AddConstraint(this);	
}

WheelConstraint::WheelConstraint()
{
}

WheelConstraint* WheelConstraint::Spawn(std::map<std::string, std::string> info)
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

	WheelConstraint* ballConstraint = new WheelConstraint();

	ballConstraint->ID = info.find("ID")->second;

	ballConstraint->AddConstraint(physicObject1, physicObject2);

	return ballConstraint;
}