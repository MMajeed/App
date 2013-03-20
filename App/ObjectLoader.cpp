#include "ObjectLoader.h"
#include "tinyxml2\tinyxml2.h"
#include "PlyFile.h"
#include "SkyBox.h"
#include "Helper.h"
#include "Transparent.h"
#include "SphericalMirror.h"
#include "FBXObject.h"

ObjectLoader* ObjectLoader::ObjectManager = 0;

void ObjectLoader::LoadXMLFile(std::string loc)
{
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

	for(auto objectNodeIter = document.FirstChildElement("Objects")->FirstChildElement("Object"); 
		objectNodeIter;
		objectNodeIter = objectNodeIter->NextSiblingElement("Object") )
	{
		std::string ID;
		std::string classType;
		std::map<std::string, std::string> info;

		for(auto childNodeIter = objectNodeIter->FirstChildElement();
			childNodeIter;
			childNodeIter = childNodeIter->NextSiblingElement())
		{
			std::string Name	= childNodeIter->Name();
			if(Name == "ID")
			{
				ID = childNodeIter->GetText();
			}
			else if(Name == "Class")
			{
				classType = childNodeIter->GetText();
				info["Class"] = childNodeIter->GetText();
			}
			else if (Name.compare(0, 3, "XYZ") == 0)
			{
				for(auto iterAttribute = childNodeIter->FirstAttribute();
					iterAttribute;
					iterAttribute = iterAttribute->Next())
				{
					std::string attributeName = iterAttribute->Name();

					info[Name+attributeName] = iterAttribute->Value();

				}
			}
			else if (Name.compare(0, 6, "Shader") == 0)
			{
				std::string X = childNodeIter->Attribute("FileName");
				info[Name+"FileName"] = X;
				std::string Y = childNodeIter->Attribute("EntryPoint");
				info[Name+"EntryPoint"] = Y;
				std::string Z = childNodeIter->Attribute("Model");
				info[Name+"Model"] = Z;
			}
			else
			{
				info[Name] = childNodeIter->GetText();
			}
		}

		if(ID == "" || classType == "")
		{
			throw std::exception("Error loading one of the objects. It didn't have a class or an ID");
		}

		this->objects[ID] = info;
	}	
}

bool ObjectLoader::Spawn(std::string name, iObjectDrawable*& object)
{
	auto objectIter = this->objects.find(name);

	if(objectIter == this->objects.end())
	{
		return false;
	}

	if(objectIter->second["Class"] == "PlyFile")
	{
		object = PlyFile::Spawn(objectIter->second);
	}
	else if(objectIter->second["Class"] == "SkyBox")
	{
		object = SkyBox::Spawn(objectIter->second);
	}
	else if(objectIter->second["Class"] == "TransparentPly")
	{
		object = Transparent::Spawn(objectIter->second);
	}
	else if(objectIter->second["Class"] == "SphericalMirror")
	{
		object = SphericalMirror::Spawn(objectIter->second);
	}
	else if(objectIter->second["Class"] == "FBXFile")
	{
		object = FBXObject::Spawn(objectIter->second);
	}
	else 
	{
		return false;
	}

	object->Init();
	return true;
}

std::vector<iObjectDrawable*> ObjectLoader::SpawnAll()
{
	std::vector<iObjectDrawable*> all;

	for(auto objectIter = this->objects.begin();
		objectIter != this->objects.end();
		++objectIter)
	{
		iObjectDrawable* newObject;
		this->Spawn(objectIter->first, newObject);
		all.push_back(newObject);
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

