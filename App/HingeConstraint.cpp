#include "HingeConstraint.h"
#include "HavokPhysics.h"
#include "Application.h"
#include "Helper.h"

void HingeConstraint::AddConstraint(IPhysicsObject* o1, IPhysicsObject* o2)
{
	HavokObject* hkO1 = dynamic_cast<HavokObject*>(o1);
	HavokObject* hkO2 = dynamic_cast<HavokObject*>(o2);

	hkpRigidBodyCinfo bodyCinfo;
	hkO1->m_RigidBody->getCinfo(bodyCinfo);

	hkVector4 halfSize;
	hkVector4 size;
	
	switch(bodyCinfo.m_shape->m_type)
	{
	//case hkcdShapeType::ShapeTypeEnum::BOX:
	case hkcdShapeType::BOX:
		{
			auto boxShape = ( hkpBoxShape *)(bodyCinfo.m_shape);
			halfSize = boxShape->getHalfExtents();
			size.setMul4(hkVector4(2,2,2,1),halfSize);
		}
		break;
	}

	// Set the pivot
	hkVector4 pivot;
	pivot.setAdd4(bodyCinfo.m_position, halfSize);
	pivot(0) -= size(0);  // Move pivot to corner of cube

	hkVector4 ax;
	if(this->axis == 0)		{	ax = hkVector4(1, 0, 0);	}
	else if(this->axis == 1){	ax = hkVector4(0, 1, 0);	}
	else if(this->axis == 2){	ax = hkVector4(0, 0, 1);	}

	// Create constraint
	hkpHingeConstraintData* hc = new hkpHingeConstraintData();
	hc->setInWorldSpace(hkO1->m_RigidBody->getTransform(), hkO2->m_RigidBody->getTransform(), pivot, ax);
	
	this->m_hkConstraint = new hkpConstraintInstance(hkO1->m_RigidBody, hkO2->m_RigidBody, hc);

	HavokPhysics::getInstance()->AddConstraint(this);	
}

HingeConstraint::HingeConstraint()
{
	this->axis = 0.0f;
}

HingeConstraint* HingeConstraint::Spawn(std::map<std::string, std::string> info)
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

	HingeConstraint* hinge = new HingeConstraint();
	
	auto iter = info.find("Axis");
	if(iter != info.end()) { hinge->axis = Helper::StringToInt(iter->second); }
	
	hinge->ID = info.find("ID")->second;

	hinge->AddConstraint(physicObject1, physicObject2);
	

	return hinge;
}