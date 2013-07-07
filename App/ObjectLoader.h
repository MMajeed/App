#ifndef __ObjectLoader__
#define __ObjectLoader__

#include <string>
#include <map>
#include "iObjectDrawable.h"
#include "LightManager.h"
#include "ObjectInfo.h"

class ObjectLoader
{
public:
	// Singelton
	static ObjectLoader* getInstance();

	void LoadXMLFile(std::string loc);
	bool ObjectLoader::Spawn(std::string name, ObjectInfo& object);
	std::map<std::string, ObjectInfo> SpawnAll();
	LightManager SetupLight();
	void SetupConstraint();

protected:
	struct ObjectMaps
	{
		std::map<std::string, std::string> Drawing;
		std::map<std::string, std::string> Physics;
		std::map<std::string, std::string> Common;
		std::map<std::string, std::string> Input;
	};
	std::map<std::string, ObjectMaps> objects;
	std::vector<std::map<std::string, std::string>> PhysicsConstraints;
	std::vector<std::map<std::string, std::string>> Lights;
	static ObjectLoader* ObjectManager;

    ObjectLoader();
    ObjectLoader(ObjectLoader const&);              
    void operator=(ObjectLoader const&); 

};

#endif // __ObjectLoader__