#ifndef SKELETAL_ANIM_H
#define SKELETAL_ANIM_H

#include "cFBXBuffer.h"

struct Frame
{
    std::vector<cFBXBuffer::JointPose>	mBones;
    float					mTime;

    Frame() : mTime(-1.0f) {}
};

class SkeletalAnimation
{
public:
	SkeletalAnimation():
		mNumBones(0),
		mDuration(0.0f)
	{
	}
	virtual ~SkeletalAnimation(){}

    std::vector<cFBXBuffer::Joint>		mSkeleton;
    int						mNumBones;

    std::vector<Frame>		mKeys;
    float					mDuration;
    std::string				mName;    
};

#endif
