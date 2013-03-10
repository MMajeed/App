#include "FBXObject.h"
#include "cFBXBuffer.h"
#include "Application.h"
#include "DX11Helper.h"
#include "Helper.h"
#include "cBuffer.h"
#include "DX11ObjectManager.h"
#include "MathHelper.h"
#include <sstream>

FBXObject::FBXObject()
{
	this->mAnimTime						= 0.0f;
	this->mAnimRate						= 1.0f;
	this->mCurrentFrame					= 0;
	this->currAnimation					= 50000;

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

FBXObject::~FBXObject()
{
}

void FBXObject::SetMesh(Mesh pMesh)
{
	this->mMesh = pMesh;
}
void FBXObject::Init()
{
	this->mCurrentGlobalPose.resize(this->mMesh.mNumBones);
	this->mBoneTransforms.resize(128);
	this->mChannelMap.resize(this->mMesh.mNumBones);
	mCurrentBones.resize(this->mMesh.mNumBones);

	for(std::size_t i = 0; i < this->mMesh.mNumBones; ++i)
	{
		this->mCurrentGlobalPose[i] = this->mMesh.mOrigGlobalPose[i];
		mCurrentBones[i] = this->mMesh.mOrigBones[i];
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
void FBXObject::Clean()
{

}
void FBXObject::UpdateDrawing(float delta)
{
	if(this->currAnimation < this->mAnimation.size())
	{
        //find the appropriate frame
        this->mAnimTime += delta * this->mAnimRate;

		if(this->mAnimTime >= this->mAnimation[this->currAnimation].mDuration)
        {
            this->mAnimTime = 0.0f;
			this->mCurrentFrame = 0;
        }

		if(this->mAnimTime > this->mAnimation[this->currAnimation].mKeys[this->mCurrentFrame].mTime)
		{
			this->mCurrentFrame += 1;
		}

        int prevFrame = 0;
		float ratio = 0;
		if(this->mCurrentFrame > 0)
		{
			prevFrame = this->mCurrentFrame - 1;
			ratio = (this->mAnimTime - this->mAnimation[this->currAnimation].mKeys[prevFrame].mTime) / (this->mAnimation[this->currAnimation].mKeys[this->mCurrentFrame].mTime
							- this->mAnimation[this->currAnimation].mKeys[prevFrame].mTime);
		}

        // we've got the frame, so update the animation   
        for(std::size_t i = 0; i < this->mMesh.mNumBones; ++i)
        {
            if(this->mChannelMap[i] != -1)
            {
				cFBXBuffer::JointPose* jntA = &(this->mAnimation[this->currAnimation].mKeys[prevFrame].mBones[this->mChannelMap[i]]);
				cFBXBuffer::JointPose* jntB = &(this->mAnimation[this->currAnimation].mKeys[this->mCurrentFrame].mBones[this->mChannelMap[i]]);

				XMStoreFloat3(&mCurrentBones[i].translation, XMVectorLerp(XMLoadFloat3(&jntA->translation), XMLoadFloat3(&jntB->translation), ratio));
                XMStoreFloat4(&mCurrentBones[i].rotation, XMQuaternionSlerp(XMLoadFloat4(&jntA->rotation), XMLoadFloat4(&jntB->rotation), ratio));
                XMStoreFloat3(&mCurrentBones[i].scale, XMVectorLerp(XMLoadFloat3(&jntA->scale), XMLoadFloat3(&jntB->scale), ratio));
            }
        }

		//update the current pose
		for(std::size_t i = 0; i < this->mMesh.mNumBones; ++i)
		{
			XMMATRIX m = mCurrentBones[i].GetTransform();
			if(this->mMesh.mSkeleton[i].parent >= 0)
			{
				XMMATRIX c = XMLoadFloat4x4(&this->mCurrentGlobalPose[this->mMesh.mSkeleton[i].parent]);
				m *= c;
			}
        
			XMStoreFloat4x4(&this->mCurrentGlobalPose[i], m);
		}
    }
}
void FBXObject::UpdateObject(float delta)
{
	UNREFERENCED_PARAMETER(delta);
}
void FBXObject::Draw()
{
	//update the current pose
    for(std::size_t i = 0; i < this->mMesh.mNumBones; ++i)
    {
		XMMATRIX invBind = XMLoadFloat4x4(&this->mMesh.mSkeleton[i].invBindPose);
		XMMATRIX currPose = XMLoadFloat4x4(&this->mCurrentGlobalPose[i]);
		XMMATRIX total = invBind * currPose;
		XMMATRIX invTotal = XMMatrixTranspose( total);
		XMFLOAT4X4 inv;
		XMStoreFloat4x4(&inv, invTotal);
	
		this->mBoneTransforms[i] = inv;
	}

    //
    // Update variables
    //
	cBuffer::cbChangeEveryFrame cb;
	XMFLOAT4X4 world = this->object.CalculateMatrix();
	cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&world));

	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;

	pImmediateContext->UpdateSubresource( this->pCBChangesEveryFrame.second, 0, NULL, &cb, 0, 0 );
	pImmediateContext->UpdateSubresource( this->pAnimBonesBuffer.second, 0, NULL, &(this->mBoneTransforms.front()), 0, 0 );
    	
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

    pImmediateContext->DrawIndexed( this->mMesh.mIndices.size(), 0, 0 );

}

void FBXObject::AddAnimation(const SkeletalAnimation& anim)
{
	this->mAnimation.push_back(anim);
}
void FBXObject::PlayAnimation(std::size_t anim)
{
	if(anim < this->mAnimation.size())
	{
		this->currAnimation		= anim;
		this->mAnimTime			= 0.0f;
		this->mCurrentFrame		= 0;

		for(std::size_t i = 0; i < this->mMesh.mNumBones; ++i)
		{
			this->mChannelMap[i] = static_cast<unsigned char>(-1);
			for(std::size_t j = 0; j < this->mAnimation[this->currAnimation].mNumBones; ++j)
			{
				if(this->mMesh.mSkeleton[i].name == this->mAnimation[this->currAnimation].mSkeleton[j].name)
				{
					this->mChannelMap[i] = static_cast<unsigned char>(j);
					break;
				}
			}
		}
	}
}
std::string FBXObject::GetPlayingAnimation() const
{
	if(this->currAnimation < this->mAnimation.size())
	{
        return(this->mAnimation[this->currAnimation].mName);
    }
    else
    {
        return("None");
    }
}
float FBXObject::GetCurrentAnimTime() const
{
    return(mAnimTime);
}
std::size_t FBXObject::GetCurrentAnimFrame() const
{
    return(this->mCurrentFrame);
}

void FBXObject::InitVertexBuffer(ID3D11Device* device)
{
	std::wstring error;

	if(!DX11ObjectManager::getInstance()->PlyBuffer.Exists(this->pVertexBuffer.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadVertexBuffer<cFBXBuffer::SimpleSkinnedVertex>(device, &(this->mMesh.mVerts.front()), this->mMesh.mVerts.size(), &(this->pVertexBuffer.second), error ))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->PlyBuffer.Add(this->pVertexBuffer.first, pVertexBuffer.second);
	}
}
void FBXObject::InitIndexBuffer(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->IndexBuffer.Exists(this->pIndexBuffer.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadIndexBuffer<WORD>(device, &(this->mMesh.mIndices.front()), this->mMesh.mIndices.size(), &(this->pIndexBuffer.second), error ))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->IndexBuffer.Add(this->pIndexBuffer.first, this->pIndexBuffer.second);
	}
}
void FBXObject::InitInputLayout(ID3D11Device* device)
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
void FBXObject::InitVertexShader(ID3D11Device* device)
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
void FBXObject::InitPixelShader(ID3D11Device* device)
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
void FBXObject::InitRastersizerState(ID3D11Device* device)
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
void FBXObject::InitCBChangesEveryFrameBuffer(ID3D11Device* device)
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
void FBXObject::InitAnimBuffer(ID3D11Device* device)
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

void FBXObject::LoadD3DStuff()
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