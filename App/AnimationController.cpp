#include "AnimationController.h"
#include "MeshAnimationManager.h"

AnimationController::AnimationController()
{
	Mesh.second = NULL;
}
AnimationController::~AnimationController()
{

}

void AnimationController::Init(std::string path)
{
	this->Mesh.first = path;
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

void AnimationController::SetAnimation(std::string path)
{
	AnimationPlayerA.Init(this->Mesh.first, path);
}

void AnimationController::Update(float delta)
{
	if(this->AnimationPlayerA.Animation.first != "")
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