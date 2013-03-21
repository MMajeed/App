#ifndef __MeshAnimmationManager__
#define __MeshAnimmationManager__

#include "Mesh.h"
#include "SkeletalAnimation.h"
#include "map"
#include <fstream>

class MeshAnimationManager
{
public:
	// Singelton
	static MeshAnimationManager* getInstance();

	void LoadMesh(std::string path);
	bool GetMesh(std::string name, Mesh*& out);
	bool ExistsMesh(std::string name);

	void LoadAnimation(std::string path);
	bool GetAnimation(std::string name, SkeletalAnimation*& out);
	bool ExistsAnimation(std::string name);

protected:
	std::map<std::string, Mesh> mapMeshObject;
	std::map<std::string, SkeletalAnimation> mapAnimationObject;

	static MeshAnimationManager* ObjectManager;

    MeshAnimationManager();
    MeshAnimationManager(MeshAnimationManager const&);              
    void operator=(MeshAnimationManager const&); 
};


#endif // __MeshAnimmationManager__