#include "DisplayMeshGPU.h"
#include "Mesh.h"
#include "SkeletalAnimation.h"
#include "cFBXBuffer.h"
#include <new>
#include "Application.h"
#include "DX11Helper.h"
#include <sstream>
#include "Helper.h"
#include "cBuffer.h"
#include "DX11ObjectManager.h"

DisplayMeshGPU::DisplayMeshGPU() :
    mAnimTime(0.0f),
    mAnimRate(1.0f),
    mCurrentFrame(0.0f)
{
	this->Shader.ShaderInput.FileName	= "ShaderFiles/7_GpuSkinShader.fx";
	this->Shader.ShaderInput.EntryPoint = "VS";
	this->Shader.ShaderInput.Mode		= "vs_4_0";
	
	this->Shader.ShaderVertex.FileName	= "ShaderFiles/7_GpuSkinShader.fx";
	this->Shader.ShaderVertex.EntryPoint = "VS";
	this->Shader.ShaderVertex.Mode		= "vs_4_0";

	this->Shader.ShaderPixel.FileName	= "ShaderFiles/7_GpuSkinShader.fx";
	this->Shader.ShaderPixel.EntryPoint = "PS";
	this->Shader.ShaderPixel.Mode		= "ps_4_0";

	this->pVertexBuffer.first			= "FBXFile";
	this->pIndexBuffer.first			= "FBXFile";
	this->pInputLayout.first			= this->Shader.ShaderInput.EntryPoint;
	this->pVertexShader.first			= this->Shader.ShaderInput.EntryPoint;
	this->pPixelShader.first			= this->Shader.ShaderInput.EntryPoint;
	this->pRastersizerState.first		= "Wire";
	this->pCBChangesEveryFrame.first	= "ChangeEveryFrame";
	this->pAnimBonesBuffer.first		= "AnimBoneBuffer";


}

DisplayMeshGPU::~DisplayMeshGPU()
{
}

void DisplayMeshGPU::SetMesh(Mesh pMesh)
{
	mMesh = pMesh;
}

void DisplayMeshGPU::Init()
{
    HRESULT hr = S_OK;
	
	mCurrentGlobalPose.resize(mMesh.mNumBones);
	mBoneTransforms.resize(128);
	mChannelMap.resize(mMesh.mNumBones);
	mCurrentBones.resize(mMesh.mNumBones);

	for(int i = 0; i < mMesh.mNumBones; ++i)
	{
		mCurrentGlobalPose[i] = mMesh.mOrigGlobalPose[i];
		mCurrentBones[i] = mMesh.mOrigBones[i];
	}

	ID3D11Device* device = (dynamic_cast<DX11App*>(App::getInstance()))->direct3d.pd3dDevice;

	this->InitVertexBuffer(device);
	this->InitIndexBuffer(device);
	this->InitInputLayout(device);
	this->InitVertexShader(device);
	this->InitPixelShader(device);
	this->InitRastersizerState(device);
	this->InitCBChangesEveryFrameBuffer(device);
	this->InitAnimBuffer(device);
	this->LoadD3DStuff();
}

void DisplayMeshGPU::Clean()
{

}

void DisplayMeshGPU::UpdateDrawing(float delta)
{
	if(mAnimation.mName != "")
    {
        float oldTime = mAnimTime;
        mAnimTime = oldTime;
        //find the appropriate frame
        mAnimTime += delta * mAnimRate;
        while(mAnimTime >= mAnimation.mDuration)
        {
            mAnimTime -= mAnimation.mDuration;
        }
        int frame = 0;
		while(mAnimTime > mAnimation.mKeys[frame].mTime && frame < mAnimation.mKeys.size())
        {
            ++frame;
        }

        if(frame >= mAnimation.mKeys.size())
        {
            frame = 0;
        }
        
        int prevFrame = 0;
		float t = 0;
		if(frame > 0)
		{
			prevFrame = frame - 1;
			t = (mAnimTime - mAnimation.mKeys[prevFrame].mTime) / (mAnimation.mKeys[frame].mTime - mAnimation.mKeys[prevFrame].mTime);
		}

		mCurrentFrame = static_cast<float>(frame) + t; //this is just for show in the title-bar

        // we've got the frame, so update the animation   
        for(int i = 0; i < mMesh.mNumBones; ++i)
        {
            if(mChannelMap[i] != -1)
            {
				cFBXBuffer::JointPose* jntA = &(mAnimation.mKeys[prevFrame].mBones[mChannelMap[i]]);
				cFBXBuffer::JointPose* jntB = &(mAnimation.mKeys[frame].mBones[mChannelMap[i]]);

				XMStoreFloat3(&mCurrentBones[i].translation, XMVectorLerp(XMLoadFloat3(&jntA->translation), XMLoadFloat3(&jntB->translation), t));
                XMStoreFloat4(&mCurrentBones[i].rotation, XMQuaternionSlerp(XMLoadFloat4(&jntA->rotation), XMLoadFloat4(&jntB->rotation), t));
                XMStoreFloat3(&mCurrentBones[i].scale, XMVectorLerp(XMLoadFloat3(&jntA->scale), XMLoadFloat3(&jntB->scale), t));
            }
        }
    }
    else
    {
        //some test rotation of bones to see the skinning work
        XMVECTOR r = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
        for(int i = 1; i < mMesh.mNumBones; ++i)
        {
            XMVECTOR q = XMLoadFloat4(&mMesh.mOrigBones[i].rotation);

            q = XMQuaternionMultiply(q, r);

            XMStoreFloat4(&mCurrentBones[i].rotation, q);
        }
    }

    //update the current pose
    for(int i = 0; i < mMesh.mNumBones; ++i)
    {
        XMMATRIX m = mCurrentBones[i].GetTransform();
        if(mMesh.mSkeleton[i].parent >= 0)
        {
            XMMATRIX c = XMLoadFloat4x4(&mCurrentGlobalPose[mMesh.mSkeleton[i].parent]);
            m *= c;
        }
        
        XMStoreFloat4x4(&mCurrentGlobalPose[i], m);
    }
}

void DisplayMeshGPU::UpdateObject(float delta)
{
	UNREFERENCED_PARAMETER(delta);
}

void DisplayMeshGPU::Draw()
{
	//update the current pose
    for(int i = 0; i < mMesh.mNumBones; ++i)
    {
		XMMATRIX invBind = XMLoadFloat4x4(&mMesh.mSkeleton[i].invBindPose);
		XMMATRIX currPose = XMLoadFloat4x4(&mCurrentGlobalPose[i]);
		XMMATRIX total = invBind * currPose;
		XMMATRIX invTotal = XMMatrixTranspose( total);
		XMFLOAT4X4 inv;
		XMStoreFloat4x4(&inv, invTotal);
	
		mBoneTransforms[i] = inv;
	}

    //
    // Update variables
    //
	cBuffer::cbChangeEveryFrame cb;
	memset(&cb, 0, sizeof(cb));
	cb.mWorld = XMMatrixTranspose( XMMatrixIdentity() );

	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;

	pImmediateContext->UpdateSubresource( this->pCBChangesEveryFrame.second, 0, NULL, &cb, 0, 0 );
	pImmediateContext->UpdateSubresource( this->pAnimBonesBuffer.second, 0, NULL, &(mBoneTransforms.front()), 0, 0 );
    	
    pImmediateContext->VSSetConstantBuffers( 2, 1, &(this->pCBChangesEveryFrame.second) );
	pImmediateContext->VSSetConstantBuffers( 3, 1, &(this->pAnimBonesBuffer.second) );

    // Set vertex buffer

    UINT stride = sizeof( cFBXBuffer::SimpleSkinnedVertex );
    UINT offset = 0;
	pImmediateContext->IASetVertexBuffers( 0, 1, &(this->pVertexBuffer.second), &stride, &offset );
	pImmediateContext->IASetIndexBuffer( this->pIndexBuffer.second, DXGI_FORMAT_R16_UINT, 0 );
    pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


    // Render a triangle
    // Set the input layout
	pImmediateContext->IASetInputLayout( this->pInputLayout.second );
	pImmediateContext->VSSetShader( this->pVertexShader.second, NULL, 0 );
	pImmediateContext->PSSetShader( this->pPixelShader.second, NULL, 0 );

	pImmediateContext->RSSetState( this->pRastersizerState.second );

    pImmediateContext->DrawIndexed( mMesh.mNumIndices, 0, 0 );

}

void DisplayMeshGPU::PlayAnimation(SkeletalAnimation anim)
{
    mAnimation = anim;
    mAnimTime = 0.0f;
    if(anim.mName != "")
    {
        for(int i = 0; i < mMesh.mNumBones; ++i)
        {
            mChannelMap[i] = -1;
            for(int j = 0; j < anim.mNumBones; ++j)
            {
                if(strcmp(mMesh.mSkeleton[i].name, anim.mSkeleton[j].name) == 0)
                {
                    mChannelMap[i] = j;
                    break;
                }
            }
        }
    }    
}

std::string DisplayMeshGPU::GetPlayingAnimation() const
{
    static char none[] = "None";
    if(mAnimation.mName != "")
    {
        return(mAnimation.mName);
    }
    else
    {
        return(none);
    }
}

float DisplayMeshGPU::GetCurrentAnimTime() const
{
    return(mAnimTime);
}

float DisplayMeshGPU::GetCurrentAnimFrame() const
{
    return(mCurrentFrame);
}

void DisplayMeshGPU::InitVertexBuffer(ID3D11Device* device)
{
	std::wstring error;

	if(!DX11ObjectManager::getInstance()->PlyBuffer.Exists(this->pVertexBuffer.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadVertexBuffer<cFBXBuffer::SimpleSkinnedVertex>(device, &(mMesh.mVerts.front()), mMesh.mNumVerts, &(this->pVertexBuffer.second), error ))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->PlyBuffer.Add(this->pVertexBuffer.first, pVertexBuffer.second);
	}
}
void DisplayMeshGPU::InitIndexBuffer(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->IndexBuffer.Exists(this->pIndexBuffer.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadIndexBuffer<WORD>(device, &(mMesh.mIndices.front()), mMesh.mNumIndices, &(this->pIndexBuffer.second), error ))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->IndexBuffer.Add(this->pIndexBuffer.first, this->pIndexBuffer.second);
	}
}
void DisplayMeshGPU::InitInputLayout(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->InputLayout.Exists(this->pInputLayout.first))
	{
		std::wstring error;
		
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "JOINTINDEX", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "JOINTWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE( layout );

		if(!DX11Helper::LoadInputLayoutFile(Shader.ShaderInput.FileName, Shader.ShaderInput.EntryPoint, Shader.ShaderInput.Mode, device, layout, numElements, &(this->pInputLayout.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->InputLayout.Add(this->pInputLayout.first, this->pInputLayout.second);
	}
}
void DisplayMeshGPU::InitVertexShader(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->VertexShader.Exists(this->pVertexShader.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadVertexShaderFile(Shader.ShaderVertex.FileName, Shader.ShaderVertex.EntryPoint, Shader.ShaderVertex.Mode, device, &(this->pVertexShader.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->VertexShader.Add(this->pVertexShader.first , this->pVertexShader.second);
	}
}
void DisplayMeshGPU::InitPixelShader(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->PixelShader.Exists(this->pPixelShader.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadPixelShaderFile(Shader.ShaderPixel.FileName, Shader.ShaderPixel.EntryPoint, Shader.ShaderPixel.Mode, device, &(this->pPixelShader.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->PixelShader.Add(this->pPixelShader.first , this->pPixelShader.second);
	}
}
void DisplayMeshGPU::InitRastersizerState(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->RastersizerState.Exists(this->pRastersizerState.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, true, true, device, &(this->pRastersizerState.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->RastersizerState.Add(this->pRastersizerState.first, this->pRastersizerState.second);
	}
}
void DisplayMeshGPU::InitCBChangesEveryFrameBuffer(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->CBuffer.Exists(this->pCBChangesEveryFrame.first))
	{
		std::wstring error;
		if(!cBuffer::LoadBuffer<cBuffer::cbChangeEveryFrame>(device, &(this->pCBChangesEveryFrame.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->CBuffer.Add(this->pCBChangesEveryFrame.first, this->pCBChangesEveryFrame.second);
	}
}
void DisplayMeshGPU::InitAnimBuffer(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->CBuffer.Exists(this->pAnimBonesBuffer.first))
	{
		std::wstring error;
		if(!cBuffer::LoadBuffer<cFBXBuffer::AnimMatrices>(device, &(this->pAnimBonesBuffer.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->CBuffer.Add(this->pAnimBonesBuffer.first, this->pAnimBonesBuffer.second);
	}
}

void DisplayMeshGPU::LoadD3DStuff()
{
	if(!DX11ObjectManager::getInstance()->PlyBuffer.Get(this->pVertexBuffer.first, this->pVertexBuffer.second)){ throw std::exception("Vertex Buffer not found"); }
	if(!DX11ObjectManager::getInstance()->IndexBuffer.Get(this->pIndexBuffer.first, this->pIndexBuffer.second)){ throw std::exception("Index Buffer not found"); }
	if(!DX11ObjectManager::getInstance()->InputLayout.Get(this->pInputLayout.first, this->pInputLayout.second)){ throw std::exception("Input Layout not found"); }
	if(!DX11ObjectManager::getInstance()->VertexShader.Get(this->pVertexShader.first, this->pVertexShader.second)){ throw std::exception("Vertex Shader not found"); }
	if(!DX11ObjectManager::getInstance()->PixelShader.Get(this->pPixelShader.first, this->pPixelShader.second)){ throw std::exception("Pixel Shader not found"); }
	if(!DX11ObjectManager::getInstance()->RastersizerState.Get(this->pRastersizerState.first, this->pRastersizerState.second)){ throw std::exception("Rastersizer State not found"); }
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->pCBChangesEveryFrame.first, this->pCBChangesEveryFrame.second)){ throw std::exception("const buffer not found"); }	
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->pAnimBonesBuffer.first, this->pAnimBonesBuffer.second)){ throw std::exception("Anim Bone buffer not found"); }	
}