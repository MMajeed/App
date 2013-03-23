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
		HalfAndHalf
	};

	AnimationController();
	virtual ~AnimationController();

	void Init(std::string meshKey);
	void SetAnimation(std::string animationKey, AnimationState as = AnimationController::SnapShot);

	void Update(float delta);

	std::pair<std::string, Mesh*>	Mesh; // Don't delete
	std::vector<XMFLOAT4X4>			CurrentGlobalPose;

	AnimationPlayer					AnimationPlayerA;
	AnimationPlayer					AnimationPlayerB;

protected:
	float						timeBetweenAnimation;
	float						finalTimeBetweenAnimation;	
	std::set<unsigned char>		BonesToMove;
	std::vector<unsigned char>	ChannelMap;

	AnimationState CurrentState;

	void UpdateOneAnimation(float delta);
	void UpdateCrossFade(float delta);
	void UpdateSnapShot(float delta);
	void UpdateHalfAndHalf(float delta);
};

#endif // __AnimationController__