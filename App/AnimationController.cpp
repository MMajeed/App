#include "AnimationController.h"
#include "MeshAnimationManager.h"

AnimationController::AnimationController()
{
	this->Mesh.second = NULL;
	this->CurrentState = AnimationController::Nothing;
	this->mRootTranslation = XMFLOAT3(0.0f, 0.0f, 0.0);
	this->mRootRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}
AnimationController::~AnimationController()
{

}

void AnimationController::Init(std::string meshKey)
{
	this->Mesh.first = meshKey;
	bool hr = MeshAnimationManager::getInstance()->GetMesh(this->Mesh.first, this->Mesh.second );
	if(!hr)
	{
		throw std::exception("Failed at finding mesh");
	}

	this->CurrentGlobalPose.resize(this->Mesh.second->mNumBones);
	this->mCurrentBones.resize(this->Mesh.second->mNumBones);
	for(std::size_t i = 0; i < this->Mesh.second->mNumBones; ++i)
	{
		this->CurrentGlobalPose[i] = this->Mesh.second->mOrigGlobalPose[i];
	}
}

void AnimationController::SetAnimation(std::string animationKey, AnimationState as, float length)
{
	if(this->AnimationPlayerA.IsSet() == true && this->CurrentState != AnimationController::Nothing)
    {
		this->AnimationPlayerB = AnimationPlayerA;

		this->timeBetweenAnimation = 0.0f;
		this->finalTimeBetweenAnimation = length;
	}
	this->AnimationPlayerA.Init(this->Mesh.first, animationKey);

	if(this->CurrentState == AnimationController::Nothing)
	{
		this->CurrentState = AnimationController::OneAnimation;
	}
	else
	{
		this->CurrentState = as;
	}
}

void AnimationController::SetPartial(std::string animationKey)
{
	this->partialAnim.play = true;
	this->partialAnim.AnimationPlayer.Init(this->Mesh.first, animationKey);

	SkeletalAnimation* animA = this->AnimationPlayerA.Animation.second;
	SkeletalAnimation* animB = this->partialAnim.AnimationPlayer.Animation.second;

	this->partialAnim.ChannelMap.resize(animA->mNumBones);
	for(std::size_t i = 0; i < animA->mNumBones; ++i)
	{
		this->partialAnim.ChannelMap[i] = static_cast<unsigned char>(-1);
		for(std::size_t j = 0; j < animB->mNumBones; ++j)
		{
			if(strcmp(animA->mSkeleton[i].name, animB->mSkeleton[j].name)  == 0)
			{
				this->partialAnim.ChannelMap[i] = static_cast<unsigned char>(j);
				break;
			}
		}
	}

	for(std::size_t i = 0; i < animA->mNumBones; ++i)
	{
		auto currentBone = animA->mSkeleton[i];
		while(currentBone.parent > 0)
		{
			if(std::string(currentBone.name) == "Chest")
			{
				this->partialAnim.BonesToMove.insert(i);
				break;
			}
			else
			{
				currentBone = animA->mSkeleton[currentBone.parent];
			}
		}
	}
}

void AnimationController::TurnoffPartial()
{
	this->partialAnim.play = false;
}
void AnimationController::Update(float delta)
{
	switch(this->CurrentState)
	{
	case AnimationController::OneAnimation:
		this->UpdateOneAnimation(delta);
		this->UpdateLoc();
		break;
	case AnimationController::CrossFade:
		this->UpdateCrossFade(delta);
		this->UpdateLocLerp();
		break;
	case AnimationController::SnapShot:
		this->UpdateSnapShot(delta);
		this->UpdateLocLerp();
		break;
	case AnimationController::TimeSync:
		this->UpdateTimeSync(delta);
		this->UpdateLocLerp();
		break;
	}

	if(this->partialAnim.AnimationPlayer.IsSet() && this->partialAnim.play == true)
	{
		this->UpdateHalfAndHalf(delta);
	}
	

	for(std::size_t i = 0; i < this->Mesh.second->mNumBones; ++i)
	{
		XMMATRIX m = this->mCurrentBones[i].GetTransform();
		if(this->Mesh.second->mSkeleton[i].parent >= 0)
		{
			const XMMATRIX& c = XMLoadFloat4x4(&this->CurrentGlobalPose[this->Mesh.second->mSkeleton[i].parent]);
			m *= c;
		}
        
		XMStoreFloat4x4(&this->CurrentGlobalPose[i], m);
	}
}

void AnimationController::UpdateOneAnimation(float delta)
{
	this->AnimationPlayerA.Play(delta);

	for(std::size_t i = 0; i < this->Mesh.second->mNumBones; ++i)
	{
		this->mCurrentBones[i] = this->AnimationPlayerA.CurrentBones[i];
	}
}
void AnimationController::UpdateCrossFade(float delta)
{
	this->timeBetweenAnimation += delta;

	if(this->finalTimeBetweenAnimation <= this->timeBetweenAnimation)
	{
		this->CurrentState = AnimationController::OneAnimation;
	}

	this->AnimationPlayerA.Play(delta);
	this->AnimationPlayerB.Play(delta);

	float t = this->timeBetweenAnimation / (this->finalTimeBetweenAnimation );

	for(std::size_t i = 0; i < this->Mesh.second->mNumBones; ++i)
	{
		cFBXBuffer::JointPose betweenJoint;

		cFBXBuffer::JointPose& jntA = this->AnimationPlayerA.CurrentBones[i];
		cFBXBuffer::JointPose& jntB = this->AnimationPlayerB.CurrentBones[i];

		XMStoreFloat3(&betweenJoint.translation, XMVectorLerp(XMLoadFloat3(&jntB.translation), XMLoadFloat3(&jntA.translation), t));
		XMStoreFloat4(&betweenJoint.rotation, XMQuaternionSlerp(XMLoadFloat4(&jntB.rotation), XMLoadFloat4(&jntA.rotation), t));
		XMStoreFloat3(&betweenJoint.scale, XMVectorLerp(XMLoadFloat3(&jntB.scale), XMLoadFloat3(&jntA.scale), t));

		this->mCurrentBones[i] =betweenJoint;
		
	}
}
void AnimationController::UpdateSnapShot(float delta)
{
	this->timeBetweenAnimation += delta;

	if(this->finalTimeBetweenAnimation <= this->timeBetweenAnimation)
	{
		this->CurrentState = AnimationController::OneAnimation;
	}

	this->AnimationPlayerA.Play(delta);
	this->AnimationPlayerB.Play(0);

	float t = this->timeBetweenAnimation / (this->finalTimeBetweenAnimation );

	for(std::size_t i = 0; i < this->Mesh.second->mNumBones; ++i)
	{
		cFBXBuffer::JointPose betweenJoint;

		cFBXBuffer::JointPose& jntA = this->AnimationPlayerA.CurrentBones[i];
		cFBXBuffer::JointPose& jntB = this->AnimationPlayerB.CurrentBones[i];

		XMStoreFloat3(&betweenJoint.translation, XMVectorLerp(XMLoadFloat3(&jntB.translation), XMLoadFloat3(&jntA.translation), t));
		XMStoreFloat4(&betweenJoint.rotation, XMQuaternionSlerp(XMLoadFloat4(&jntB.rotation), XMLoadFloat4(&jntA.rotation), t));
		XMStoreFloat3(&betweenJoint.scale, XMVectorLerp(XMLoadFloat3(&jntB.scale), XMLoadFloat3(&jntA.scale), t));

		this->mCurrentBones[i] =betweenJoint;
		
	}
}
void AnimationController::UpdateTimeSync(float delta)
{
	this->timeBetweenAnimation += delta;

	if(this->finalTimeBetweenAnimation <= this->timeBetweenAnimation)
	{
		this->CurrentState = AnimationController::OneAnimation;
	}

	this->AnimationPlayerB.Play(delta);
	this->AnimationPlayerA.SetCurrentPhase(this->AnimationPlayerB.GetCurrentPhase());
	this->AnimationPlayerA.Play(0);

	float t = this->timeBetweenAnimation / (this->finalTimeBetweenAnimation );

	for(std::size_t i = 0; i < this->Mesh.second->mNumBones; ++i)
	{
		cFBXBuffer::JointPose betweenJoint;

		cFBXBuffer::JointPose& jntA = this->AnimationPlayerA.CurrentBones[i];
		cFBXBuffer::JointPose& jntB = this->AnimationPlayerB.CurrentBones[i];

		XMStoreFloat3(&betweenJoint.translation, XMVectorLerp(XMLoadFloat3(&jntB.translation), XMLoadFloat3(&jntA.translation), t));
		XMStoreFloat4(&betweenJoint.rotation, XMQuaternionSlerp(XMLoadFloat4(&jntB.rotation), XMLoadFloat4(&jntA.rotation), t));
		XMStoreFloat3(&betweenJoint.scale, XMVectorLerp(XMLoadFloat3(&jntB.scale), XMLoadFloat3(&jntA.scale), t));

		this->mCurrentBones[i] =betweenJoint;
	}
}
void AnimationController::UpdateHalfAndHalf(float delta)
{
	this->partialAnim.AnimationPlayer.Play(delta);

	for(std::size_t i = 0; i < this->Mesh.second->mNumBones; ++i)
	{
		XMMATRIX m;
		
		if(this->partialAnim.BonesToMove.find(static_cast<unsigned char>(i)) != this->partialAnim.BonesToMove.end())
		{
			this->mCurrentBones[i] = this->partialAnim.AnimationPlayer.CurrentBones[i];
		}
	}
}

void AnimationController::UpdateLoc()
{
	this->mRootTranslation = this->AnimationPlayerA.mRootTranslation;
	this->mRootRotation = this->AnimationPlayerA.mRootRotation;
}
void AnimationController::UpdateLocLerp()
{
	XMFLOAT3 pos1 = this->AnimationPlayerA.mRootTranslation;
	XMFLOAT4 rot1 = this->AnimationPlayerA.mRootRotation;

	XMFLOAT3 pos2 = this->AnimationPlayerB.mRootTranslation;
	XMFLOAT4 rot2 = this->AnimationPlayerB.mRootRotation;
	
	float t = this->timeBetweenAnimation / (this->finalTimeBetweenAnimation );

	XMStoreFloat3(&this->mRootTranslation, XMVectorLerp(XMLoadFloat3(&pos2), XMLoadFloat3(&pos1), t));
	XMStoreFloat4(&this->mRootRotation, XMQuaternionSlerp(XMLoadFloat4(&rot2), XMLoadFloat4(&rot1), t));
}