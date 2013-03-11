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
	void LoadMeshRecursive(FbxNode* pNode);
	void LoadSkinInfo(FbxNode* pNode, int vertOffset, int numVerts);
	void LoadBindPose();
	void LoadSkeletonRecursive(FbxNode* pNode, int parent);
	int CountSkeletonRecursive(FbxNode* pNode);
	void TriangulateRecursive(FbxNode* pNode);
};

#endif //__FBXMESHLOADER__
