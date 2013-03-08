#ifndef DISPLAY_MESH_GPU_H
#define DISPLAY_MESH_GPU_H

#include "Entity.h"
#include "HelperFuncs.h"

class Mesh;
class SkeletalAnimation;

class DisplayMeshGPU : public Entity
{
public:
    DisplayMeshGPU();
    virtual ~DisplayMeshGPU();

	void SetMesh(Mesh* pMesh);
    virtual HRESULT Initialize();
    virtual void Update(float delta);
    virtual void Draw();

    void SetDrawBindPose(bool bindPose) { mDrawBindPose = bindPose; }
    bool GetDrawBindPose() const { return(mDrawBindPose); }

    void SetFrameInterpolate(bool interpolate) { mFrameInterpolate = interpolate; }
    bool GetFrameInterpolate() const { return(mFrameInterpolate); }

    void SetDebugRotation(float rot) { mDebugRotation = rot; }
    float GetDebugRotation() const { return(mDebugRotation); }

    void PlayAnimation(SkeletalAnimation* anim);
    
    char* GetPlayingAnimation() const;
    float GetCurrentAnimTime() const;
    float GetCurrentAnimFrame() const;

    void SetAnimRate(float rate) { mAnimRate = rate; }
    float GetAnimRate() const { return(mAnimRate); }

protected:
	Mesh*                   mMesh; //not owned by this class
    SkeletalAnimation*      mAnimation; //not owned by this class
    unsigned char*          mChannelMap;

    JointPose*              mCurrentBones;
    _XMFLOAT4X4*            mCurrentGlobalPose;

	_XMFLOAT4X4*            mBoneTransforms;

    bool                    mDrawBindPose;
    bool                    mFrameInterpolate;
    float                   mDebugRotation;

    float                   mAnimTime;
    float                   mAnimRate;
    float                   mCurrentFrame;

    //DX stuff
    ID3D11VertexShader*     mVertexShader;
    ID3D11PixelShader*      mPixelShader;
    ID3D11InputLayout*      mVertexLayout;
    ID3D11Buffer*           mVertexBuffer;
    ID3D11Buffer*           mIndexBuffer;
    ID3D11Buffer*           mConstantBuffer;
	ID3D11Buffer*           mAnimBonesBuffer;
	ID3D11RasterizerState*   g_pRasterState;
};

#endif
