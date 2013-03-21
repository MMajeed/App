#ifndef __AnimationController__
#define __AnimationController__

#include "AnimationPlayer.h"

class AnimationController
{
public:
	AnimationController();
	virtual ~AnimationController();

	void Init(std::string meshKey);
	void SetAnimation(std::string animationKey);

	void Update(float delta);

	std::pair<std::string, Mesh*>	Mesh; // Don't delete
	std::vector<XMFLOAT4X4>			CurrentGlobalPose;

	AnimationPlayer					AnimationPlayerA;
	AnimationPlayer					AnimationPlayerB;

	bool					betweenAnimation;
	float					timeBetweenAnimation;
	float					finalTimeBetweenAnimation;	
};

#endif // __AnimationController__