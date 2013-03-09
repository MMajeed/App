#ifndef DISPLAY_MESH_GPU_H
#define DISPLAY_MESH_GPU_H

#include "HelperFuncs.h"
#include "iObjectDrawable.h"
#include "ShaderFiles.h"

class Mesh;
class SkeletalAnimation;

class DisplayMeshGPU : public iObjectDrawable
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

    DisplayMeshGPU();
    virtual ~DisplayMeshGPU();

	void SetMesh(Mesh* pMesh);

    void PlayAnimation(SkeletalAnimation* anim);
    
    char* GetPlayingAnimation() const;
    float GetCurrentAnimTime() const;
    float GetCurrentAnimFrame() const;

    void SetAnimRate(float rate) { mAnimRate = rate; }
    float GetAnimRate() const { return(mAnimRate); }

	Mesh*                   mMesh;				//not owned by this class
    SkeletalAnimation*      mAnimation;			//not owned by this class
    unsigned char*          mChannelMap;

    JointPose*              mCurrentBones;
    _XMFLOAT4X4*            mCurrentGlobalPose;

	_XMFLOAT4X4*            mBoneTransforms;


    float                   mAnimTime;
    float                   mAnimRate;
    float                   mCurrentFrame;

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
