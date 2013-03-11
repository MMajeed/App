#ifndef __FBXMESHLOADER__
#define __FBXMESHLOADER__

#include "Mesh.h"
#include <fbxsdk.h>

class FBXMeshLoader
{
public:
	FbxManager* g_FbxSdkManager;

	Mesh LoadMesh(std::string path);	
	
	FBXMeshLoader();
	virtual ~FBXMeshLoader();

	void WriteToFile(std::string path);
protected:
	FbxScene* lScene;
	Mesh pMesh;
	void LoadMeshFromScene();
	void TriangulateRecursive(FbxNode* pNode);
	int CountSkeletonRecursive(FbxNode* pNode);
	void LoadSkeletonRecursive(FbxNode* pNode, int parent);
	void LoadBindPose();
	void LoadMeshRecursive(FbxNode* pNode);
	void LoadSkinInfo(FbxNode* pNode, int vertOffset, int numVerts);
};

#endif //__FBXMESHLOADER__
