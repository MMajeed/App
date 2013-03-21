#include "FBXObject.h"
#include "cFBXBuffer.h"
#include "Application.h"
#include "DX11Helper.h"
#include "Helper.h"
#include "cBuffer.h"
#include "DX11ObjectManager.h"
#include "MathHelper.h"
#include <sstream>
#include "MeshAnimationManager.h"

FBXObject::FBXObject()
{
	this->currAnimation					= 50000;

	this->Shader.ShaderInput.FileName	= "../Resources/ShaderFiles/7_GpuSkinShader.fx";
	this->Shader.ShaderInput.EntryPoint = "VS";
	this->Shader.ShaderInput.Mode		= "vs_4_0";
	
	this->Shader.ShaderVertex.FileName	= "../Resources/ShaderFiles/7_GpuSkinShader.fx";
	this->Shader.ShaderVertex.EntryPoint = "VS";
	this->Shader.ShaderVertex.Mode		= "vs_4_0";

	this->Shader.ShaderPixel.FileName	= "../Resources/ShaderFiles/7_GpuSkinShader.fx";
	this->Shader.ShaderPixel.EntryPoint = "PS";
	this->Shader.ShaderPixel.Mode		= "ps_4_0";

	this->pMeshVertexBuffer.first			= "FBXFile";
	this->pMeshIndexBuffer.first			= "FBXFile";
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

void FBXObject::Init()
{
	AnimController.Init(this->MeshKey);

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
	this->AnimController.Update(delta);
}
void FBXObject::UpdateObject(float delta)
{
	this->object.Update(delta);
}
void FBXObject::Draw()
{
	static std::vector<XMFLOAT4X4> mBoneTransforms(128);
	for(std::size_t i = 0; i < this->AnimController.Mesh.second->mNumBones; ++i)
    {
		XMMATRIX invBind = XMLoadFloat4x4(&this->AnimController.Mesh.second->mSkeleton[i].invBindPose);
		XMMATRIX currPose = XMLoadFloat4x4(&this->AnimController.CurrentGlobalPose[i]);
		XMMATRIX total = invBind * currPose;
		XMMATRIX invTotal = XMMatrixTranspose( total);
		XMFLOAT4X4 inv;
		XMStoreFloat4x4(&inv, invTotal);
	
		mBoneTransforms[i] = inv;
	}

	cBuffer::cbChangeEveryFrame cb;
	XMFLOAT4X4 world = this->object.CalculateMatrix();
	cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&world));
	cb.colour.diffuse = this->object.Colour.Diffuse;
	cb.colour.ambient = this->object.Colour.Ambient;
	cb.colour.spec = this->object.Colour.Spec;

	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;

	pImmediateContext->UpdateSubresource( this->pCBChangesEveryFrame.second, 0, NULL, &cb, 0, 0 );
	pImmediateContext->UpdateSubresource( this->pAnimBonesBuffer.second, 0, NULL, &(mBoneTransforms.front()), 0, 0 );
    	
    pImmediateContext->VSSetConstantBuffers( 2, 1, &(this->pCBChangesEveryFrame.second) );
	pImmediateContext->VSSetConstantBuffers( 3, 1, &(this->pAnimBonesBuffer.second) );

	pImmediateContext->PSSetConstantBuffers( 2, 1, &(this->pCBChangesEveryFrame.second) );

    UINT stride = sizeof( cFBXBuffer::SimpleSkinnedVertex );
    UINT offset = 0;
	pImmediateContext->IASetVertexBuffers( 0, 1, &(this->pMeshVertexBuffer.second), &stride, &offset );
	pImmediateContext->IASetIndexBuffer( this->pMeshIndexBuffer.second, DXGI_FORMAT_R16_UINT, 0 );
    pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	pImmediateContext->IASetInputLayout( this->pInputLayout.second );
	pImmediateContext->VSSetShader( this->pVertexShader.second, NULL, 0 );
	pImmediateContext->PSSetShader( this->pPixelShader.second, NULL, 0 );

	pImmediateContext->RSSetState( this->pRastersizerState.second );

    pImmediateContext->DrawIndexed( this->AnimController.Mesh.second->mIndices.size(), 0, 0 );

}

void FBXObject::LoadMesh(std::string path)
{
	this->MeshKey = path;
	MeshAnimationManager::getInstance()->LoadMesh(path);
}
void FBXObject::AddAnimation(std::string path)
{
	this->AnimationKey.push_back(path);
	MeshAnimationManager::getInstance()->LoadAnimation(path);
}
void FBXObject::PlayAnimation(std::size_t anim)
{
	if(anim < this->AnimationKey.size())
	{
		this->currAnimation = anim;
		this->AnimController.SetAnimation(this->AnimationKey[this->currAnimation]);
	}
}
std::string FBXObject::GetPlayingAnimation() const
{
	if(this->currAnimation < this->AnimationKey.size())
	{
		return(this->AnimController.AnimationPlayerA.Animation.second->mName);
    }
    else
    {
        return("None");
    }
}
void FBXObject::SetAnimRate(float rate) { /*this->AnimationPlayer.AnimRate = rate;*/ }
float FBXObject::GetAnimRate() const { /*return(this->AnimationPlayer.AnimRate);*/ return 0.0f; }
float FBXObject::GetCurrentAnimTime() const
{
	/*return(this->AnimationPlayer.AnimTime);*/ return 0.0f;
}
std::size_t FBXObject::GetCurrentAnimFrame() const
{
    /*return(this->AnimationPlayer.CurrentFrame);*/
	return 0;
}

void FBXObject::InitVertexBuffer(ID3D11Device* device)
{
	std::wstring error;

	if(!DX11ObjectManager::getInstance()->PlyBuffer.Exists(this->pMeshVertexBuffer.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadVertexBuffer<cFBXBuffer::SimpleSkinnedVertex>(device, &(this->AnimController.Mesh.second->mVerts.front()), this->AnimController.Mesh.second->mVerts.size(), &(this->pMeshVertexBuffer.second), error ))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->PlyBuffer.Add(this->pMeshVertexBuffer.first, pMeshVertexBuffer.second);
	}
}
void FBXObject::InitIndexBuffer(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->IndexBuffer.Exists(this->pMeshIndexBuffer.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadIndexBuffer<WORD>(device, &(this->AnimController.Mesh.second->mIndices.front()), this->AnimController.Mesh.second->mIndices.size(), &(this->pMeshIndexBuffer.second), error ))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->IndexBuffer.Add(this->pMeshIndexBuffer.first, this->pMeshIndexBuffer.second);
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
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "JOINTINDEX", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "JOINTWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
	if(!DX11ObjectManager::getInstance()->PlyBuffer.Get(this->pMeshVertexBuffer.first, this->pMeshVertexBuffer.second)){ throw std::exception("Vertex Buffer not found"); }
	if(!DX11ObjectManager::getInstance()->IndexBuffer.Get(this->pMeshIndexBuffer.first, this->pMeshIndexBuffer.second)){ throw std::exception("Index Buffer not found"); }
	if(!DX11ObjectManager::getInstance()->InputLayout.Get(this->pInputLayout.first, this->pInputLayout.second)){ throw std::exception("Input Layout not found"); }
	if(!DX11ObjectManager::getInstance()->VertexShader.Get(this->pVertexShader.first, this->pVertexShader.second)){ throw std::exception("Vertex Shader not found"); }
	if(!DX11ObjectManager::getInstance()->PixelShader.Get(this->pPixelShader.first, this->pPixelShader.second)){ throw std::exception("Pixel Shader not found"); }
	if(!DX11ObjectManager::getInstance()->RastersizerState.Get(this->pRastersizerState.first, this->pRastersizerState.second)){ throw std::exception("Rastersizer State not found"); }
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->pCBChangesEveryFrame.first, this->pCBChangesEveryFrame.second)){ throw std::exception("const buffer not found"); }	
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->pAnimBonesBuffer.first, this->pAnimBonesBuffer.second)){ throw std::exception("Anim Bone buffer not found"); }	
}

FBXObject* FBXObject::Spawn(std::map<std::string, std::string> info)
{
	FBXObject* FBX = new FBXObject;
	
	auto iter = info.find("Mesh");
	if(iter == info.end()){throw std::exception("No Mesh was included in the object");}
	FBX->LoadMesh(iter->second);

	iter = info.find("Animation1");
	if(iter != info.end()){ FBX->AddAnimation(iter->second); }

	iter = info.find("Animation2");
	if(iter != info.end()){ FBX->AddAnimation(iter->second); }

	iter = info.find("Animation3");
	if(iter != info.end()){ FBX->AddAnimation(iter->second); }


	iter = info.find("ShaderVertexEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Input Entry point Name was included in the object");}
	FBX->Shader.ShaderVertex.EntryPoint = iter->second;
	FBX->Shader.ShaderInput.EntryPoint = iter->second;

	iter = info.find("ShaderVertexModel");
	if(iter == info.end()){throw std::exception("No Shader Input Model point Name was included in the object");}
	FBX->Shader.ShaderVertex.Mode = iter->second;
	FBX->Shader.ShaderInput.Mode = iter->second;

	// Shader Vertex
	iter = info.find("ShaderVertexFileName");
	if(iter == info.end()){throw std::exception("No Shader Vertex File Name was included in the object");}
	FBX->Shader.ShaderVertex.FileName = iter->second;	
	FBX->pVertexShader.first = iter->second;
	FBX->Shader.ShaderInput.FileName = iter->second;	
	FBX->pInputLayout.first = iter->second;


	iter = info.find("ShaderVertexEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Vertex Entry point Name was included in the object");}
	FBX->Shader.ShaderVertex.EntryPoint = iter->second;

	iter = info.find("ShaderVertexModel");
	if(iter == info.end()){throw std::exception("No Shader Vertex Model point Name was included in the object");}
	FBX->Shader.ShaderVertex.Mode = iter->second;

	// Shader Pixel
	iter = info.find("ShaderPixelFileName");
	if(iter == info.end()){throw std::exception("No Shader Pixel File Name was included in the object");}
	FBX->Shader.ShaderPixel.FileName = iter->second;
	FBX->pPixelShader.first = iter->second;

	iter = info.find("ShaderPixelEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Pixel Entry point Name was included in the object");}
	FBX->Shader.ShaderPixel.EntryPoint = iter->second;

	iter = info.find("ShaderPixelModel");
	if(iter == info.end()){throw std::exception("No Shader Pixel Model point Name was included in the object");}
	FBX->Shader.ShaderPixel.Mode = iter->second;

	// Scale XYZ
	iter = info.find("XYZScaleX");
	if(iter != info.end()) { FBX->object.Scale.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZScaleY");
	if(iter != info.end()) { FBX->object.Scale.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZScaleZ");
	if(iter != info.end()) { FBX->object.Scale.z = Helper::StringToFloat(iter->second); } 
	
	// Location
	iter = info.find("XYZLocationX");
	if(iter != info.end()) { FBX->object.Pos.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZLocationY");
	if(iter != info.end()) { FBX->object.Pos.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZLocationZ");
	if(iter != info.end()) { FBX->object.Pos.z = Helper::StringToFloat(iter->second); } 

	// XYZRotation
	iter = info.find("XYZRotationX");
	if(iter != info.end()) { FBX->object.Rot.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZRotationY");
	if(iter != info.end()) { FBX->object.Rot.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZRotationZ");
	if(iter != info.end()) { FBX->object.Rot.z = Helper::StringToFloat(iter->second); } 

	// XYZDiffuse
	iter = info.find("XYZDiffuseX");
	if(iter != info.end()) { FBX->object.Colour.Diffuse.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZDiffuseY");
	if(iter != info.end()) { FBX->object.Colour.Diffuse.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZDiffuseZ");
	if(iter != info.end()) { FBX->object.Colour.Diffuse.z = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZDiffuseW");
	if(iter != info.end()) { FBX->object.Colour.Diffuse.w = Helper::StringToFloat(iter->second); } 

	// XYZAmbent
	iter = info.find("XYZAmbentX");
	if(iter != info.end()) { FBX->object.Colour.Ambient.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZAmbentY");
	if(iter != info.end()) { FBX->object.Colour.Ambient.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZAmbentZ");
	if(iter != info.end()) { FBX->object.Colour.Ambient.z = Helper::StringToFloat(iter->second); } 

	// XYZAmbent
	iter = info.find("XYZSpecX");
	if(iter != info.end()) { FBX->object.Colour.Spec.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZSpecY");
	if(iter != info.end()) { FBX->object.Colour.Spec.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZSpecZ");
	if(iter != info.end()) { FBX->object.Colour.Spec.z = Helper::StringToFloat(iter->second); } 

	return FBX;
}