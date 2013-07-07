#ifndef __AnimationController__
#define __AnimationController__

#include "AnimationPlayer.h"
#include <set>

class AnimationController
{
public:
	enum AnimationState
	{
		Nothing,
		OneAnimation,
		CrossFade,
		SnapShot,
		TimeSync
	};

	AnimationController();
	virtual ~AnimationController();

	void Init(std::string meshKey);
	void SetAnimation(std::string animationKey, AnimationState as = AnimationController::SnapShot, float length = 2.f);
	void SetPartial(std::string animationKey);
	void TurnoffPartial();
	void Update(float delta);

	std::pair<std::string, Mesh*>		Mesh; // Don't delete
	std::vector<XMFLOAT4X4>				CurrentGlobalPose;
	std::vector<cFBXBuffer::JointPose>	mCurrentBones;

	AnimationPlayer					AnimationPlayerA;
	AnimationPlayer					AnimationPlayerB;
	
    XMFLOAT3                mRootTranslation;
    XMFLOAT4                mRootRotation;

	struct PartialAnim
	{
		bool play;
		std::vector<unsigned char>	ChannelMap;
		AnimationPlayer				AnimationPlayer;
		std::set<unsigned char>		BonesToMove;
		PartialAnim(): play(false){}
	};

	PartialAnim partialAnim;
protected:
	float						timeBetweenAnimation;
	float						finalTimeBetweenAnimation;	

	

	AnimationState CurrentState;

	void UpdateOneAnimation(float delta);
	void UpdateCrossFade(float delta);
	void UpdateSnapShot(float delta);
	void UpdateTimeSync(float delta);
	void UpdateHalfAndHalf(float delta);

	void UpdateLoc();
	void UpdateLocLerp();
};

#endif // __AnimationController__