#ifndef __AnimationController__
#define __AnimationController__

#include "AnimationPlayer.h"

class AnimationController
{
public:
	AnimationController();
	virtual ~AnimationController();

	void Init(std::string path);
	void SetAnimation(std::string path);

	void Update(float delta);

	std::pair<std::string, Mesh*>	Mesh; // Don't delete
	std::vector<XMFLOAT4X4>			CurrentGlobalPose;
	AnimationPlayer					AnimationPlayerA;
	
};

#endif // __AnimationController__