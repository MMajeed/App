#ifndef MESH_H
#define MESH_H

#include "HelperFuncs.h"

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	SimpleSkinnedVertex*    mVerts;
	WORD*					mIndices;
    int						mNumVerts;
	int						mNumIndices;

    Joint*					mSkeleton;
    JointPose*				mOrigBones;
    _XMFLOAT4X4*			mOrigGlobalPose;
    int						mNumBones;

    SimpleSkinnedVertex*	mOrigVerts;
};

#endif
