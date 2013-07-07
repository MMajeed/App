#include "VertexHavok.h"
#include "HavokPhysics.h"
#include "Helper.h"

void VertexHavok::Init(ObjectInfo* o)
{
	hkRefPtr<hkpTriangleShape> verticesTriangle = new hkpTriangleShape();

	hkArray<hkVector4> vertices;

	for(std::size_t i = 0; i < this->PlyBuffer.indices.size(); ++i)
	{
		hkReal x, y, z;
		x = this->PlyBuffer.vertices.at(this->PlyBuffer.indices.at(i)).Pos.x;
		y = this->PlyBuffer.vertices.at(this->PlyBuffer.indices.at(i)).Pos.y;
		z = this->PlyBuffer.vertices.at(this->PlyBuffer.indices.at(i)).Pos.z;

		hkVector4 newVertice(x, y, z, 0.0f);

		vertices.pushBack(newVertice);
	}

	// This structure provides the build options
	hkpConvexVerticesShape::BuildConfig config;
	config.m_convexRadius = 0.1f;

	// Creates a convex vertices shape from these 4 vertices and the convex radius.
	// By default, the shape will be shrunk during construction so that the resulting "shell" matches the input vertices.		
	
	hkRefPtr<hkpConvexVerticesShape> shape = new hkpConvexVerticesShape( hkStridedVertices(&vertices[0], this->PlyBuffer.indices.size()), config );

	hkpRigidBodyCinfo bodyCinfo;
	bodyCinfo.m_shape = shape;
	bodyCinfo.m_solverDeactivation = bodyCinfo.SOLVER_DEACTIVATION_MEDIUM;
	// Place box somewhere above floor...
	bodyCinfo.m_position = this->startingPosition;
	bodyCinfo.m_rotation = hkQuaternion(this->startingRotation);
	bodyCinfo.m_restitution = this->restitution;
	bodyCinfo.m_friction = this->friction;
	if(moveable)
	{
		//  Calculate the mass properties for the shape
		hkMassProperties massProperties;
		hkpInertiaTensorComputer::computeShapeVolumeMassProperties(shape, mass, massProperties);
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

VertexHavok::VertexHavok()
{
	this->startingPosition = hkVector4(0.0f, 0.0f, 0.0f);
	this->mass = 0.0f;
	this->friction = 0.0f;
	this->restitution = 0.0f;
	this->moveable = true;
}

VertexHavok* VertexHavok::Spawn(std::map<std::string, std::string> info)
{
	VertexHavok* newVertex = new VertexHavok();

	std::wstring error;
	auto iter = info.find("PlyFile");
	if(iter == info.end()){throw std::exception("No ply file was included in the object");}
	bool hr = PlyBuffer::LoadFromPlyFile(Helper::stringToWstring(iter->second), newVertex->PlyBuffer, error); 
	if(!hr){ throw std::exception(Helper::WStringtoString(L"Error loading ply file: " + error).c_str()); }	
	// XYZLocation
	iter = info.find("XYZLocationX");
	if(iter != info.end()) { newVertex->startingPosition.setComponent(0, Helper::StringToFloat(iter->second)); } 	
	iter = info.find("XYZLocationY");
	if(iter != info.end()) { newVertex->startingPosition.setComponent(1, Helper::StringToFloat(iter->second)); } 	
	iter = info.find("XYZLocationZ");
	if(iter != info.end()) { newVertex->startingPosition.setComponent(2, Helper::StringToFloat(iter->second)); } 	
	
	float roll = 0; float yaw = 0; float pitch = 0;

	// XYZRotation
	iter = info.find("XYZRotationX");
	if(iter != info.end()) { pitch = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZRotationY");
	if(iter != info.end()) { yaw = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZRotationZ");
	if(iter != info.end()) { roll = Helper::StringToFloat(iter->second); } 	

	newVertex->startingRotation.setFromEulerAngles(roll, pitch, yaw);

	// mass
	iter = info.find("Mass");
	if(iter != info.end()) { newVertex->mass = Helper::StringToFloat(iter->second); } 	

	// restitution
	iter = info.find("Restitution");
	if(iter != info.end()) { newVertex->restitution = Helper::StringToFloat(iter->second); } 	

	// Friction
	iter = info.find("Friction");
	if(iter != info.end()) { newVertex->friction = Helper::StringToFloat(iter->second); } 	
	
	// Moveable
	iter = info.find("Moveable");
	if(iter != info.end()) { newVertex->moveable = Helper::ToLower(iter->second) == "true"; } 	

	return newVertex;
}
