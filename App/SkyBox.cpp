#include "BasicObject.h"
#include "DX11ObjectManager.h"
#include "App.h"
#include "DX11App.h"
#include "DX11Helper.h"
#include "Helper.h"
#include "cBuffer.h"
#include "VertexBuffer.h"
#include "SkyBox.h"
#include <sstream>

SkyBox::SkyBox()
{
	this->pRastersizerState.first = "InsideOut";
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

void SkyBox::UpdateObject(float delta)
{
	UNREFERENCED_PARAMETER(delta);

	// Set the pos to where the camera is so the skymap dosn't move
	this->object.Pos = App::getInstance()->camera.Eye();
}

float SkyBox::GetOrder()
{
	return 1000000.0f;
}

SkyBox* SkyBox::Spawn(std::map<std::string, std::string> info)
{
	SkyBox* newSkyBox = new SkyBox;
	
	std::wstring error;

	auto iter = info.find("PlyFile");
	if(iter == info.end()){throw std::exception("No ply file was included in the object");}
	bool hr = VertexBuffer::LoadFromPlyFile(Helper::stringToWstring(iter->second), newSkyBox->vertexBuffer, error); 
	if(!hr){ throw std::exception("Error loading ply file"); }	
	newSkyBox->pVertexBuffer.first = iter->second;
	newSkyBox->pIndexBuffer.first = iter->second;

	// Shader Input
	iter = info.find("ShaderInputFileName");
	if(iter == info.end()){throw std::exception("No Shader Input File Name was included in the object");}
	newSkyBox->ShaderInput.FileName = iter->second;
	newSkyBox->pInputLayout.first = iter->second;

	iter = info.find("ShaderInputEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Input Entry point Name was included in the object");}
	newSkyBox->ShaderInput.EntryPoint = iter->second;

	iter = info.find("ShaderInputModel");
	if(iter == info.end()){throw std::exception("No Shader Input Model point Name was included in the object");}
	newSkyBox->ShaderInput.Mode = iter->second;

	// Shader Vertex
	iter = info.find("ShaderVertexFileName");
	if(iter == info.end()){throw std::exception("No Shader Vertex File Name was included in the object");}
	newSkyBox->ShaderVertex.FileName = iter->second;	
	newSkyBox->pVertexShader.first = iter->second;

	iter = info.find("ShaderVertexEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Vertex Entry point Name was included in the object");}
	newSkyBox->ShaderVertex.EntryPoint = iter->second;

	iter = info.find("ShaderVertexModel");
	if(iter == info.end()){throw std::exception("No Shader Vertex Model point Name was included in the object");}
	newSkyBox->ShaderVertex.Mode = iter->second;

	// Shader Pixel
	iter = info.find("ShaderPixelFileName");
	if(iter == info.end()){throw std::exception("No Shader Pixel File Name was included in the object");}
	newSkyBox->ShaderPixel.FileName = iter->second;
	newSkyBox->pPixelShader.first = iter->second;

	iter = info.find("ShaderPixelEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Pixel Entry point Name was included in the object");}
	newSkyBox->ShaderPixel.EntryPoint = iter->second;

	iter = info.find("ShaderPixelModel");
	if(iter == info.end()){throw std::exception("No Shader Pixel Model point Name was included in the object");}
	newSkyBox->ShaderPixel.Mode = iter->second;

	// Scale XYZ
	iter = info.find("XYZScaleX");
	if(iter != info.end()) { newSkyBox->object.Scale.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZScaleY");
	if(iter != info.end()) { newSkyBox->object.Scale.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZScaleZ");
	if(iter != info.end()) { newSkyBox->object.Scale.z = Helper::StringToFloat(iter->second); } 
	
	// Location
	iter = info.find("XYZLocationX");
	if(iter != info.end()) { newSkyBox->object.Pos.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZLocationY");
	if(iter != info.end()) { newSkyBox->object.Pos.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZLocationZ");
	if(iter != info.end()) { newSkyBox->object.Pos.z = Helper::StringToFloat(iter->second); } 

	// XYZRotation
	iter = info.find("XYZRotationX");
	if(iter != info.end()) { newSkyBox->object.Rot.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZRotationY");
	if(iter != info.end()) { newSkyBox->object.Rot.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZRotationZ");
	if(iter != info.end()) { newSkyBox->object.Rot.z = Helper::StringToFloat(iter->second); } 

	// XYZOrbit
	iter = info.find("XYZOrbitX");
	if(iter != info.end()) { newSkyBox->object.Orbit.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZOrbitY");
	if(iter != info.end()) { newSkyBox->object.Orbit.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZOrbitZ");
	if(iter != info.end()) { newSkyBox->object.Orbit.z = Helper::StringToFloat(iter->second); } 


	iter = info.find("CubeMap");
	if(iter != info.end()) { newSkyBox->pCubeMap.first = iter->second; } 

	newSkyBox->pRastersizerState.first    = "SkyBox";

	return newSkyBox;
}