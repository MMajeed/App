#ifndef MESH_H
#define MESH_H

#include "cFBXBuffer.h"

class Mesh
{
public:
	Mesh():
    mNumBones(0){}
	virtual ~Mesh(){}

	std::vector<cFBXBuffer::SimpleSkinnedVertex>	mVerts;
	std::vector<WORD>								mIndices;

    std::vector<cFBXBuffer::Joint>					mSkeleton;
    std::vector<cFBXBuffer::JointPose>				mOrigBones;
    std::vector<XMFLOAT4X4>							mOrigGlobalPose;
    std::size_t										mNumBones;

    std::vector<cFBXBuffer::SimpleSkinnedVertex>	mOrigVerts;
};

#endif
