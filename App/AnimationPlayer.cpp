#include "AnimationPlayer.h"
#include "MeshAnimationManager.h"

AnimationPlayer::AnimationPlayer()
{
	this->Animation.second = NULL;
	this->AnimTime         = 0.0f;
	this->AnimRate         = 1.0f;
	this->CurrentFrame     = 0;
	this->PreviousFrame    = 0;
}
AnimationPlayer::~AnimationPlayer()
{
}

void AnimationPlayer::Init(std::string meshPath, std::string animationPath)
{
	Mesh* mesh;

	bool hr = MeshAnimationManager::getInstance()->GetMesh(meshPath, mesh);
	if(!hr)
	{
		std::exception("Failed to load mesh");
	}

	hr = MeshAnimationManager::getInstance()->GetAnimation(animationPath, this->Animation.second);
	if(!hr)
	{
		std::exception("Failed to load animation");
	}

	this->AnimTime = 0.0f;
	this->CurrentFrame = 0;

	this->ChannelMap.resize(mesh->mNumBones);
	this->CurrentBones.resize(mesh->mNumBones);

	for(std::size_t i = 0; i < mesh->mNumBones; ++i)
	{
		this->ChannelMap[i] = static_cast<unsigned char>(-1);
		for(std::size_t j = 0; j < this->Animation.second->mNumBones; ++j)
		{
			if(strcmp(mesh->mSkeleton[i].name, this->Animation.second->mSkeleton[j].name)  == 0)
			//if(this->mMesh->mSkeleton[i].name == this->Animation->mSkeleton[j].name)
			{
				this->ChannelMap[i] = static_cast<unsigned char>(j);
				break;
			}
		}
	}
}

void AnimationPlayer::Play(float delta)
{
	//find the appropriate frame
	this->AnimTime += delta * this->AnimRate;
	
	if(this->AnimTime < 0)
	{
		return; // No time change so don't do anything
	}

	if(this->AnimTime >= this->Animation.second->mDuration)
    {
        this->AnimTime = 0.0f;
		this->CurrentFrame = 0;
    }
	else if(this->AnimTime < 0.0f)
    {
        this->AnimTime = this->Animation.second->mDuration;
		this->CurrentFrame = this->Animation.second->mKeys.size() - 1;
    }

	std::size_t prevFrame = this->CurrentFrame;
	if(this->AnimTime < this->Animation.second->mKeys[this->CurrentFrame].mTime)
	{
		do
		{
			this->CurrentFrame -= 1;
		}while(this->AnimTime < this->Animation.second->mKeys[this->CurrentFrame].mTime);
	}
	else if(this->AnimTime > this->Animation.second->mKeys[this->CurrentFrame].mTime)
	{
		do
		{
			this->CurrentFrame += 1;
		}while(this->AnimTime > this->Animation.second->mKeys[this->CurrentFrame].mTime);
	}

	if(prevFrame != this->CurrentFrame)
	{
		this->PreviousFrame = prevFrame;
	}

	float ratio = (this->AnimTime - this->Animation.second->mKeys[this->PreviousFrame].mTime) / 
					(this->Animation.second->mKeys[this->CurrentFrame].mTime - this->Animation.second->mKeys[this->PreviousFrame].mTime);
		
	for(std::size_t i = 0; i < this->CurrentBones.size(); ++i)
    {
        if(this->ChannelMap[i] != -1)
        {
			const cFBXBuffer::JointPose& jntA = this->Animation.second->mKeys[this->PreviousFrame].mBones[this->ChannelMap[i]];
			const cFBXBuffer::JointPose& jntB = this->Animation.second->mKeys[this->CurrentFrame].mBones[this->ChannelMap[i]];

			auto lerpedTranslation = XMVectorLerp(XMLoadFloat3(&jntA.translation), XMLoadFloat3(&jntB.translation), ratio);
			XMStoreFloat3(&(this->CurrentBones[i].translation), lerpedTranslation);
			auto slerpedRotation = XMQuaternionSlerp(XMLoadFloat4(&jntA.rotation), XMLoadFloat4(&jntB.rotation), ratio);
            XMStoreFloat4(&(this->CurrentBones[i].rotation), slerpedRotation);
			auto lerpedScale = XMVectorLerp(XMLoadFloat3(&jntA.scale), XMLoadFloat3(&jntB.scale), ratio);
            XMStoreFloat3(&(this->CurrentBones[i].scale), lerpedScale);
        }
    }
}