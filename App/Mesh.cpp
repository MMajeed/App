#include "Mesh.h"

Mesh::Mesh() :
	mVerts(NULL),
	mIndices(NULL),
	mNumVerts(0),
	mNumIndices(0),
    mSkeleton(NULL),
    mOrigBones(NULL),
    mOrigGlobalPose(NULL),
    mNumBones(0),
    mOrigVerts(NULL)
{
}

Mesh::~Mesh()
{
	if(mVerts) delete [] mVerts;
	if(mIndices) delete [] mIndices;

    if(mSkeleton) delete [] mSkeleton;
    if(mOrigBones) delete [] mOrigBones;
    if(mOrigGlobalPose) delete [] mOrigGlobalPose;

    if(mOrigVerts) delete [] mOrigVerts;
}

