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

protected:
	std::map<std::string, std::map<std::string, std::string>> objects;
	std::vector<std::map<std::string, std::string>> Lights;
	static ObjectLoader* ObjectManager;

    ObjectLoader();
    ObjectLoader(ObjectLoader const&);              
    void operator=(ObjectLoader const&); 

};

#endif // __ObjectLoader__