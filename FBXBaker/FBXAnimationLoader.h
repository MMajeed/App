#ifndef __FBXANIMATIONLOADER__
#define __FBXANIMATIONLOADER__

#include "Animation.h"
#include <fbxsdk.h>

class FBXAnimationLoader
{
public:
	FbxManager* g_FbxSdkManager;

	SkeletalAnimation LoadAnimation(std::string path, int orig, int start, int end);	
	
	FBXAnimationLoader();
	virtual ~FBXAnimationLoader();

	void WriteToFile(std::string path);
protected:
	SkeletalAnimation pAnim;
	int CountSkeletonRecursive(FbxNode* pNode);
	void LoadSkeletonRecursive(FbxNode* pNode, cFBXBuffer::Joint* pSkeleton, std::size_t& numBones, int parent);
	void LoadAnimationTake(FbxTakeInfo* pTake, FbxScene* pScene);
	void FlattenHeirarchyRecursive(FbxNode* pNode, FbxNode** pFlattenedNodes, int* numBones, int parent);
};

#endif //__FBXANIMATIONLOADER__
