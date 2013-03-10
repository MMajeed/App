#ifndef DISPLAY_MESH_GPU_H
#define DISPLAY_MESH_GPU_H

#include "cFBXBuffer.h"
#include "iObjectDrawable.h"
#include "ShaderFiles.h"
#include "Mesh.h"
#include "SkeletalAnimation.h"

class FBXObject : public iObjectDrawable
{
public:
	virtual void Init();
	virtual void Clean();
	virtual void UpdateDrawing(float delta);
	virtual void UpdateObject(float delta);
	virtual void Draw();
	virtual float GetOrder(){ return 1000.0f; }

	virtual void InitVertexBuffer(ID3D11Device* device);
	virtual void InitIndexBuffer(ID3D11Device* device);
	virtual void InitInputLayout(ID3D11Device* device);
	virtual void InitVertexShader(ID3D11Device* device);
	virtual void InitPixelShader(ID3D11Device* device);
	virtual void InitRastersizerState(ID3D11Device* device);
	virtual void InitCBChangesEveryFrameBuffer(ID3D11Device* device);
	virtual void InitAnimBuffer(ID3D11Device* device);

    FBXObject();
    virtual ~FBXObject();

	void SetMesh(Mesh pMesh);

	void AddAnimation(const SkeletalAnimation& anim);

    void PlayAnimation(std::size_t anim);    
    std::string GetPlayingAnimation() const;
    float GetCurrentAnimTime() const;
    std::size_t GetCurrentAnimFrame() const;

    void SetAnimRate(float rate) { mAnimRate = rate; }
    float GetAnimRate() const { return(mAnimRate); }

	Mesh							mMesh;
	std::vector<SkeletalAnimation>	mAnimation;
	std::size_t						currAnimation;
    std::vector<unsigned char>		mChannelMap;

    std::vector<cFBXBuffer::JointPose>			mCurrentBones;
    std::vector<XMFLOAT4X4>						mCurrentGlobalPose;
	std::vector<XMFLOAT4X4>						mBoneTransforms;

    float                   mAnimTime;
    float                   mAnimRate;
    std::size_t             mCurrentFrame;

	std::pair<std::string, ID3D11Buffer*> 			pVertexBuffer;
	std::pair<std::string, ID3D11Buffer*>			pIndexBuffer;
	std::pair<std::string, ID3D11InputLayout*>		pInputLayout;
	std::pair<std::string, ID3D11VertexShader*>		pVertexShader;
	std::pair<std::string, ID3D11PixelShader*>		pPixelShader;
	std::pair<std::string, ID3D11RasterizerState*>	pRastersizerState;
	std::pair<std::string, ID3D11Buffer*>			pCBChangesEveryFrame;
	std::pair<std::string, ID3D11Buffer*>			pAnimBonesBuffer;

	void LoadD3DStuff();

	ShaderFiles Shader;
};

#endif
