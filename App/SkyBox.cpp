#include "BasicObject.h"
#include "DX11ObjectManager.h"
#include "App.h"
#include "DX11App.h"
#include "DX11Helper.h"
#include "Helper.h"
#include "cBuffer.h"
#include "VertexBuffer.h"
#include "SkyBox.h"

SkyBox::SkyBox(std::string cubeMap)
{	
	this->pVertexBuffer.first        = "PlyFiles/Sphere_Smooth_3.ply";
	this->pIndexBuffer.first         = "PlyFiles/Sphere_Smooth_3.ply";
	this->pCBChangesEveryFrame.first = "CBChangeEveryFrame";
	this->pInputLayout.first         = "ShaderFiles/6_SkyMap.fx";
	this->pVertexShader.first        = "ShaderFiles/6_SkyMap.fx";
	this->pPixelShader.first         = "ShaderFiles/6_SkyMap.fx";
	this->pRastersizerState.first    = "SkyBox";	
	this->pCubeMap.first 			 = cubeMap;
	std::wstring error;
	
	bool hr = VertexBuffer::LoadFromPlyFile(L"PlyFiles/Sphere_Smooth_3.ply", this->vertexBuffer,error); 
	if(!hr)
	{
		throw std::exception("Error loading ply file");
	}	

	this->object.Scale = XMFLOAT4(5000.0f, 5000.0f, 5000.0f, 1.0f);
}

void SkyBox::InitRastersizerState(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->RastersizerState.Exists(this->pRastersizerState.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID, true, true, device, &(this->pRastersizerState.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->RastersizerState.Add(this->pRastersizerState.first, this->pRastersizerState.second);
	}
}

void SkyBox::Update(float delta)
{
}
