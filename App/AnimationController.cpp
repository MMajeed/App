#include "AnimationController.h"
#include "MeshAnimationManager.h"

AnimationController::AnimationController()
{
	Mesh.second = NULL;
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

void AnimationController::SetAnimation(std::string animationKey)
{
	if(AnimationPlayerA.IsSet() == true)
    {
		AnimationPlayerB = AnimationPlayerA;

		betweenAnimation = true;
		timeBetweenAnimation = 0.0f;
		finalTimeBetweenAnimation = 1.f;
	}
	AnimationPlayerA.Init(this->Mesh.first, animationKey);
}

void AnimationController::Update(float delta)
{
	if(this->AnimationPlayerA.IsSet() == true)
	{
		if(this->betweenAnimation == true)
		{
			this->timeBetweenAnimation += delta;

			if(this->finalTimeBetweenAnimation <= this->timeBetweenAnimation)
			{
				this->betweenAnimation = false;
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
		else
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
	}
}