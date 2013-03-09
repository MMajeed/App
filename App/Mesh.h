#ifndef MESH_H
#define MESH_H

#include "cFBXBuffer.h"

class Mesh
{
public:
	Mesh():
	mNumVerts(0),
	mNumIndices(0),
    mNumBones(0){}
	virtual ~Mesh(){}

	std::vector<cFBXBuffer::SimpleSkinnedVertex>	mVerts;
	std::vector<WORD>					mIndices;
    int									mNumVerts;
	int									mNumIndices;

    std::vector<cFBXBuffer::Joint>					mSkeleton;
    std::vector<cFBXBuffer::JointPose>				mOrigBones;
    std::vector<XMFLOAT4X4>				mOrigGlobalPose;
    int									mNumBones;

    std::vector<cFBXBuffer::SimpleSkinnedVertex>	mOrigVerts;
};

#endif
