#include "AnimationController.h"
#include "MeshAnimationManager.h"

AnimationController::AnimationController()
{
	this->Mesh.second = NULL;
	this->CurrentState = AnimationController::Nothing;
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

	for(std::size_t i = 0; i < this->Mesh.second->mNumBones; ++i)
	{
		this->CurrentGlobalPose[i] = this->Mesh.second->mOrigGlobalPose[i];
	}
}

void AnimationController::SetAnimation(std::string animationKey, AnimationState as)
{
	if(this->AnimationPlayerA.IsSet() == true && this->CurrentState != AnimationController::Nothing)
    {
		this->AnimationPlayerB = AnimationPlayerA;

		this->timeBetweenAnimation = 0.0f;
		this->finalTimeBetweenAnimation = 2.f;
	}
	this->AnimationPlayerA.Init(this->Mesh.first, animationKey);

	if(this->CurrentState == AnimationController::Nothing)
	{
		this->CurrentState = AnimationController::OneAnimation;
	}
	else
	{
		this->CurrentState = as;

		if(as == AnimationController::HalfAndHalf)
		{
			SkeletalAnimation* animA = this->AnimationPlayerB.Animation.second;
			SkeletalAnimation* animB = this->AnimationPlayerB.Animation.second;

			this->ChannelMap.resize(animA->mNumBones);
			for(std::size_t i = 0; i < animA->mNumBones; ++i)
			{
				this->ChannelMap[i] = static_cast<unsigned char>(-1);
				for(std::size_t j = 0; j < animB->mNumBones; ++j)
				{
					if(strcmp(animA->mSkeleton[i].name, animB->mSkeleton[j].name)  == 0)
					{
						this->ChannelMap[i] = static_cast<unsigned char>(j);
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
						this->BonesToMove.insert(i);
						break;
					}
					else
					{
						currentBone = animA->mSkeleton[currentBone.parent];
					}
				}
			}
		}
	}
}

void AnimationController::Update(float delta)
{
	switch(this->CurrentState)
	{
	case AnimationController::OneAnimation:
		this->UpdateOneAnimation(delta);
		break;
	case AnimationController::CrossFade:
		this->UpdateCrossFade(delta);
		break;
	case AnimationController::SnapShot:
		this->UpdateSnapShot(delta);
		break;
	case AnimationController::HalfAndHalf:
		this->UpdateHalfAndHalf(delta);
		break;
	}
}

void AnimationController::UpdateOneAnimation(float delta)
{
	this->AnimationPlayerA.Play(delta);

	for(std::size_t i = 0; i < this->Mesh.second->mNumBones; ++i)
	{
		XMMATRIX m = this->AnimationPlayerA.CurrentBones[i].GetTransform();
		if(this->Mesh.second->mSkeleton[i].parent >= 0)
		{
			const XMMATRIX& c = XMLoadFloat4x4(&this->CurrentGlobalPose[this->Mesh.second->mSkeleton[i].parent]);
			m *= c;
		}
        
		XMStoreFloat4x4(&this->CurrentGlobalPose[i], m);
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

		XMMATRIX m = betweenJoint.GetTransform();
		if(this->Mesh.second->mSkeleton[i].parent >= 0)
		{
			const XMMATRIX& c = XMLoadFloat4x4(&this->CurrentGlobalPose[this->Mesh.second->mSkeleton[i].parent]);
			m *= c;
		}
        
		XMStoreFloat4x4(&this->CurrentGlobalPose[i], m);
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

		XMMATRIX m = betweenJoint.GetTransform();
		if(this->Mesh.second->mSkeleton[i].parent >= 0)
		{
			const XMMATRIX& c = XMLoadFloat4x4(&this->CurrentGlobalPose[this->Mesh.second->mSkeleton[i].parent]);
			m *= c;
		}
        
		XMStoreFloat4x4(&this->CurrentGlobalPose[i], m);
	}
}
void AnimationController::UpdateHalfAndHalf(float delta)
{
	this->AnimationPlayerA.Play(delta);
	this->AnimationPlayerB.Play(delta);

	for(std::size_t i = 0; i < this->Mesh.second->mNumBones; ++i)
	{
		XMMATRIX m;
		
		if(this->BonesToMove.find(static_cast<unsigned char>(i)) != this->BonesToMove.end())
		{
			m = this->AnimationPlayerA.CurrentBones[i].GetTransform();
		}
		else
		{
			m = this->AnimationPlayerB.CurrentBones[this->ChannelMap[i]].GetTransform();
		}

		if(this->Mesh.second->mSkeleton[i].parent >= 0)
		{
			const XMMATRIX& c = XMLoadFloat4x4(&this->CurrentGlobalPose[this->Mesh.second->mSkeleton[i].parent]);
			m *= c;
		}
        
		XMStoreFloat4x4(&this->CurrentGlobalPose[i], m);
	}
}