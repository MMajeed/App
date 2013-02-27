#include "ObjectLoader.h"
#include "tinyxml2\tinyxml2.h"
#include "PlyFile.h"
#include "SkyBox.h"
#include "Helper.h"

ObjectLoader* ObjectLoader::ObjectManager = 0;

void ObjectLoader::LoadXMLFile(std::string loc)
{
	tinyxml2::XMLDocument document;
	document.LoadFile(loc.c_str());
	if(document.Error() == true)
	{
		throw std::exception(("Failed at loading xml file " + loc + "\n" + document.GetErrorStr1() + "\n" + document.GetErrorStr2()).c_str());
	}

	for(auto objectNodeIter = document.FirstChildElement("Objects")->FirstChildElement("Object"); 
		objectNodeIter;
		objectNodeIter = objectNodeIter->NextSiblingElement() )
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
				if(childNodeIter->BoolAttribute("X"))
				{
					std::string X = childNodeIter->Attribute("X");
					info[Name+"X"] = X;
				}
				if(childNodeIter->BoolAttribute("Y"))
				{
					std::string Y = childNodeIter->Attribute("Y");
					info[Name+"Y"] = Y;
				}
				if(childNodeIter->BoolAttribute("Z"))
				{
					std::string Z = childNodeIter->Attribute("Z");
					info[Name+"Z"] = Z;
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

