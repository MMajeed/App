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
	this->VertexBufferID        = "PlyFiles/Sphere_Smooth_3.ply";
	this->IndexBufferID         = "PlyFiles/Sphere_Smooth_3.ply";
	this->InputLayoutID         = "ShaderFiles/6_SkyMap.fx";
	this->VertexShaderID        = "ShaderFiles/6_SkyMap.fx";
	this->PixelShaderID         = "ShaderFiles/6_SkyMap.fx";
	this->RastersizerStateID    = "SkyBox";	
	this->CBChangesEveryFrameID = "CBChangeEveryFrame";
	this->CubeMapIDs			= cubeMap;
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
	if(!DX11ObjectManager::getInstance()->RastersizerState.Exists(this->RastersizerStateID))
	{
		std::wstring error;
		ID3D11RasterizerState* rasterizerState;
		if(!DX11Helper::LoadRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID, true, true, device, &rasterizerState, error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->RastersizerState.Add(this->RastersizerStateID, rasterizerState);
	}
}

void SkyBox::Update(float delta)
{
	Camera camera = App::getInstance()->camera;

	//this->object.Pos = camera.eye;
}
