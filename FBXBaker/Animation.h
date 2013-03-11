#ifndef __SKELETAL_ANIM__
#define __SKELETAL_ANIM__

#include "FBXBuffer.h"
#include <vector>

struct Frame
{
    std::vector<cFBXBuffer::JointPose>	mBones;
    float								mTime;

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
    std::size_t							mNumBones;

    std::vector<Frame>		mKeys;
    float					mDuration;
    std::string				mName;    
};

#endif
