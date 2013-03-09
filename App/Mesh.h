#ifndef MESH_H
#define MESH_H

#include "HelperFuncs.h"

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	SimpleSkinnedVertex*				mVerts;
	WORD*								mIndices;
    int									mNumVerts;
	int									mNumIndices;

    std::vector<Joint>					mSkeleton;
    std::vector<JointPose>				mOrigBones;
    std::vector<XMFLOAT4X4>				mOrigGlobalPose;
    int									mNumBones;

    std::vector<SimpleSkinnedVertex>	mOrigVerts;
};

#endif
