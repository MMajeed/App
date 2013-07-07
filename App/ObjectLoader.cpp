#include "ObjectLoader.h"
#include "tinyxml2\tinyxml2.h"
#include "PlyFile.h"
#include "SkyBox.h"
#include "Helper.h"
#include "Transparent.h"
#include "SphericalMirror.h"
#include "FBXObject.h"
#include "PlayerInput.h"
#include "BoxHavok.h"
#include "SphereHavok.h"
#include "VertexHavok.h"
#include "CarInput.h"
#include "BallConstraint.h"
#include "HingeConstraint.h"
#include "WheelConstraint.h"

ObjectLoader* ObjectLoader::ObjectManager = 0;

void ObjectLoader::LoadXMLFile(std::string loc)
{
	this->objects.clear();
	this->Lights.clear();

	tinyxml2::XMLDocument document;
	document.LoadFile(loc.c_str());
	if(document.Error() == true)
	{
		throw std::exception(("Failed at loading xml file " + loc + "\n" + document.GetErrorStr1() + "\n" + document.GetErrorStr2()).c_str());
	}
	for(auto objectNodeIter = document.FirstChildElement("Objects")->FirstChildElement("Light"); 
		objectNodeIter;
		objectNodeIter = objectNodeIter->NextSiblingElement("Light") )
	{
		std::map<std::string, std::string> info;

		for(auto childNodeIter = objectNodeIter->FirstChildElement();
			childNodeIter;
			childNodeIter = childNodeIter->NextSiblingElement())
		{
			std::string Name	= childNodeIter->Name();
			if (Name.compare(0, 3, "XYZ") == 0)
			{
				for(auto iterAttribute = childNodeIter->FirstAttribute();
					iterAttribute;
					iterAttribute = iterAttribute->Next())
				{
					std::string attributeName = iterAttribute->Name();

					info[Name+attributeName] = iterAttribute->Value();

				}
			}
			else
			{
				info[Name] = childNodeIter->GetText();
			}
		}
		
		this->Lights.push_back(info);
	}

	for(auto objectNodeIter = document.FirstChildElement("Objects")->FirstChildElement("PhysicsConstraint"); 
		objectNodeIter;
		objectNodeIter = objectNodeIter->NextSiblingElement("PhysicsConstraint") )
	{
		std::map<std::string, std::string> info;

		for(auto childNodeIter = objectNodeIter->FirstChildElement();
			childNodeIter;
			childNodeIter = childNodeIter->NextSiblingElement())
		{
			std::string Name = childNodeIter->Name();
			if (Name.compare(0, 3, "XYZ") == 0)
			{
				for(auto iterAttribute = childNodeIter->FirstAttribute();
					iterAttribute;
					iterAttribute = iterAttribute->Next())
				{
					std::string attributeName = iterAttribute->Name();

					info[Name+attributeName] = iterAttribute->Value();

				}
			}
			else
			{
				info[Name] = childNodeIter->GetText();
			}
		}
		
		this->PhysicsConstraints.push_back(info);
	}

	for(auto objectNodeIter = document.FirstChildElement("Objects")->FirstChildElement("Object"); 
		objectNodeIter;
		objectNodeIter = objectNodeIter->NextSiblingElement("Object") )
	{
		std::string ID;
		ObjectMaps newObject;

		for(auto childNodeIter = objectNodeIter->FirstChildElement();
			childNodeIter;
			childNodeIter = childNodeIter->NextSiblingElement())
		{
			std::string Name	= childNodeIter->Name();
			if(Name == "ID")
			{
				ID = childNodeIter->GetText();
			}
			else if (Name.compare(0, 3, "XYZ") == 0)
			{
				for(auto iterAttribute = childNodeIter->FirstAttribute();
					iterAttribute;
					iterAttribute = iterAttribute->Next())
				{
					std::string attributeName = iterAttribute->Name();

					newObject.Common[Name+attributeName] = iterAttribute->Value();

				}
			}
			else if(Name == "Draw")
			{
				for(auto drawNodeIter = childNodeIter->FirstChildElement();
					drawNodeIter;
					drawNodeIter = drawNodeIter->NextSiblingElement())
				{
					Name = drawNodeIter->Name();
					if (Name.compare(0, 3, "XYZ") == 0)
					{
						for(auto iterAttribute = drawNodeIter->FirstAttribute();
							iterAttribute;
							iterAttribute = iterAttribute->Next())
						{
							std::string attributeName = iterAttribute->Name();

							newObject.Drawing[Name+attributeName] = iterAttribute->Value();

						}
					}
					else if (Name.compare(0, 6, "Shader") == 0)
					{
						std::string X = drawNodeIter->Attribute("FileName");
						newObject.Drawing[Name+"FileName"] = X;
						std::string Y = drawNodeIter->Attribute("EntryPoint");
						newObject.Drawing[Name+"EntryPoint"] = Y;
						std::string Z = drawNodeIter->Attribute("Model");
						newObject.Drawing[Name+"Model"] = Z;
					}
					else
					{
						newObject.Drawing[Name] = drawNodeIter->GetText();
					}
				}
			}
			else if(Name == "Physics")
			{
				for(auto physicsIter = childNodeIter->FirstChildElement();
					physicsIter;
					physicsIter = physicsIter->NextSiblingElement())
				{
					Name = physicsIter->Name();
					if (Name.compare(0, 3, "XYZ") == 0)
					{
						for(auto iterAttribute = physicsIter->FirstAttribute();
							iterAttribute;
							iterAttribute = iterAttribute->Next())
						{
							std::string attributeName = iterAttribute->Name();

							newObject.Physics[Name+attributeName] = iterAttribute->Value();

						}
					}
					else
					{
						newObject.Physics[Name] = physicsIter->GetText();
					}
				}
			}
			else if(Name == "Input")
			{
				for(auto physicsIter = childNodeIter->FirstChildElement();
					physicsIter;
					physicsIter = physicsIter->NextSiblingElement())
				{
					Name = physicsIter->Name();
					if (Name.compare(0, 3, "XYZ") == 0)
					{
						for(auto iterAttribute = physicsIter->FirstAttribute();
							iterAttribute;
							iterAttribute = iterAttribute->Next())
						{
							std::string attributeName = iterAttribute->Name();

							newObject.Input[Name+attributeName] = iterAttribute->Value();

						}
					}
					else
					{
						newObject.Input[Name] = physicsIter->GetText();
					}
				}
			}
		}

		if(ID == "")
		{
			throw std::exception("Error loading one of the objects. It didn't have a class or an ID");
		}

		this->objects[ID] = newObject;
	}	
}

bool ObjectLoader::Spawn(std::string name, ObjectInfo& object)
{
	auto objectIter = this->objects.find(name);

	if(objectIter == this->objects.end())
	{
		return false;
	}

	auto info = objectIter->second;

	info.Drawing.insert(info.Common.begin(), info.Common.end());
	info.Physics.insert(info.Common.begin(), info.Common.end());

	if(objectIter->second.Drawing["Class"] == "PlyFile")
	{
		object.ObjectDrawable = PlyFile::Spawn(info.Drawing);
	}
	else if(objectIter->second.Drawing["Class"] == "SkyBox")
	{
		object.ObjectDrawable = SkyBox::Spawn(info.Drawing);
	}
	else if(objectIter->second.Drawing["Class"] == "TransparentPly")
	{
		object.ObjectDrawable = Transparent::Spawn(info.Drawing);
	}
	else if(objectIter->second.Drawing["Class"] == "SphericalMirror")
	{
		object.ObjectDrawable = SphericalMirror::Spawn(info.Drawing);
	}
	else if(objectIter->second.Drawing["Class"] == "FBXFile")
	{
		object.ObjectDrawable = FBXObject::Spawn(info.Drawing);
	}
	object.ObjectDrawable->Init();


	if(objectIter->second.Physics["Class"] == "Box")
	{
		object.Physics = BoxHavok::Spawn(info.Physics);
		object.Physics->Init(&object);
	}
	else if(objectIter->second.Physics["Class"] == "Sphere")
	{
		object.Physics = SphereHavok::Spawn(info.Physics);		
		object.Physics->Init(&object);
	}
	else if(objectIter->second.Physics["Class"] == "Vertex")
	{
		object.Physics = VertexHavok::Spawn(info.Physics);		
		object.Physics->Init(&object);
	}

	if(objectIter->second.Input["Class"] == "Vechile")
	{
		object.InputObject = new CarInput(&object);
	}
	else if(objectIter->second.Input["Class"] == "PlayerInput")
	{
		object.InputObject = new PlayerInput(&object);
	}


	return true;
}

std::map<std::string, ObjectInfo> ObjectLoader::SpawnAll()
{
	std::map<std::string, ObjectInfo> all;

	for(auto objectIter = this->objects.begin();
		objectIter != this->objects.end();
		++objectIter)
	{
		ObjectInfo& newObject = all[objectIter->first];
		this->Spawn(objectIter->first, newObject);
	}
	return all;
}

LightManager ObjectLoader::SetupLight()
{
	LightManager lightManagerReturn;

	for(std::size_t i = 0; i < this->Lights.size(); ++i)
	{
		//XYZDiffuse
		auto iter = this->Lights.at(i).find("XYZDiffuseX");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].diffuse.x = Helper::StringToFloat(iter->second); } 	
		iter = this->Lights.at(i).find("XYZDiffuseY");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].diffuse.y = Helper::StringToFloat(iter->second); } 
		iter = this->Lights.at(i).find("XYZDiffuseZ");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].diffuse.z = Helper::StringToFloat(iter->second); } 

		//XYZAmbient
		iter = this->Lights.at(i).find("XYZAmbientX");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].ambient.x = Helper::StringToFloat(iter->second); } 	
		iter = this->Lights.at(i).find("XYZAmbientY");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].ambient.y = Helper::StringToFloat(iter->second); } 
		iter = this->Lights.at(i).find("XYZAmbientZ");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].ambient.z = Helper::StringToFloat(iter->second); } 

		//XYZSpec
		iter = this->Lights.at(i).find("XYZSpecX");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].spec.x = Helper::StringToFloat(iter->second); } 	
		iter = this->Lights.at(i).find("XYZSpecY");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].spec.y = Helper::StringToFloat(iter->second); } 
		iter = this->Lights.at(i).find("XYZSpecZ");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].spec.z = Helper::StringToFloat(iter->second); } 

		//XYZPos
		iter = this->Lights.at(i).find("XYZPosX");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].pos.x = Helper::StringToFloat(iter->second); } 	
		iter = this->Lights.at(i).find("XYZPosY");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].pos.y = Helper::StringToFloat(iter->second); } 
		iter = this->Lights.at(i).find("XYZPosZ");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].pos.z = Helper::StringToFloat(iter->second); } 

		//XYZDir
		iter = this->Lights.at(i).find("XYZDirX");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].dir.x = Helper::StringToFloat(iter->second); } 	
		iter = this->Lights.at(i).find("XYZDirY");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].dir.y = Helper::StringToFloat(iter->second); } 
		iter = this->Lights.at(i).find("XYZDirZ");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].dir.z = Helper::StringToFloat(iter->second); } 

		//XYZAttenuation
		iter = this->Lights.at(i).find("XYZAttenuationX");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].attenuation.x = Helper::StringToFloat(iter->second); } 	
		iter = this->Lights.at(i).find("XYZAttenuationY");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].attenuation.y = Helper::StringToFloat(iter->second); } 
		iter = this->Lights.at(i).find("XYZAttenuationZ");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].attenuation.z = Helper::StringToFloat(iter->second); } 

		// Power
		iter = this->Lights.at(i).find("Power");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].power = Helper::StringToFloat(iter->second); } 	

		// Range
		iter = this->Lights.at(i).find("Range");
		if(iter != this->Lights.at(i).end()) { lightManagerReturn[i].range = Helper::StringToFloat(iter->second); } 	

		// Parallel
		iter = this->Lights.at(i).find("Type");
		if(iter != this->Lights.at(i).end())
		{
			if(iter->second == "None")		
			{	
				lightManagerReturn[i].type = Light::None;
			}
			else if(iter->second == "Parallel")	
			{
				lightManagerReturn[i].type = Light::Parallel;	
			}
			else if(iter->second == "Point")
			{
				lightManagerReturn[i].type = Light::Point;	
			}
			else if(iter->second == "Spot")
			{
				lightManagerReturn[i].type = Light::Spot;	
			}
		}
	}

	return lightManagerReturn;
}

void ObjectLoader::SetupConstraint()
{
	for(auto physicsConstraint = this->PhysicsConstraints.begin();
		physicsConstraint != this->PhysicsConstraints.end();
		++physicsConstraint)
	{
		if((*physicsConstraint)["Class"] == "Ball")
		{
			BallConstraint::Spawn(*physicsConstraint);
		}
		else if((*physicsConstraint)["Class"] == "Hinge")
		{
			HingeConstraint::Spawn(*physicsConstraint);
		}
		else if((*physicsConstraint)["Class"] == "Wheel")
		{
			WheelConstraint::Spawn(*physicsConstraint);
		}
	}
}

ObjectLoader* ObjectLoader::getInstance()
{
	if(ObjectLoader::ObjectManager == 0)
	{
		ObjectLoader::ObjectManager = new ObjectLoader;
	}

    return ObjectLoader::ObjectManager;
}
ObjectLoader::ObjectLoader()
{

}

