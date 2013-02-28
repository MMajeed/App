#include "DX11ObjectManager.h"
#include "App.h"
#include "DX11App.h"
#include "DX11Helper.h"
#include "Helper.h"
#include "cBuffer.h"
#include "VertexBuffer.h"
#include "Transparent.h"
#include <sstream>

void Transparent::DrawObject()
{
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
		
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	//pImmediateContext->OMSetBlendState(this->pTransparent.second, blendFactor, 0xffffffff);

	pImmediateContext->DrawIndexed( this->vertexBuffer.indices.size() * 3, 0, 0 );

	//pImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
}
void Transparent::Init()
{
	PlyFile::Init();

	ID3D11Device* device = (dynamic_cast<DX11App*>(App::getInstance()))->direct3d.pd3dDevice;

	this->LoadTransparency(device);

	if(!DX11ObjectManager::getInstance()->BelnderState.Get(this->pTransparent.first, this->pTransparent.second)){ throw std::exception("Transparent blender not found"); }
}
void Transparent::LoadTransparency(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->BelnderState.Exists(this->pTransparent.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadTransparent(device, &(this->pTransparent.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->BelnderState.Add(this->pTransparent.first , this->pTransparent.second);
	}
}
Transparent::Transparent()
{
	this->pTransparent.first = "Transparent";
}
Transparent* Transparent::Spawn(std::map<std::string, std::string> info)
{
	Transparent* newTransparent = new Transparent;
	
	std::wstring error;

	auto iter = info.find("PlyFile");
	if(iter == info.end()){throw std::exception("No ply file was included in the object");}
	bool hr = VertexBuffer::LoadFromPlyFile(Helper::stringToWstring(iter->second), newTransparent->vertexBuffer, error); 
	if(!hr){ throw std::exception("Error loading ply file"); }	
	newTransparent->pVertexBuffer.first = iter->second;
	newTransparent->pIndexBuffer.first = iter->second;

	// Shader Input
	iter = info.find("ShaderInputFileName");
	if(iter == info.end()){throw std::exception("No Shader Input File Name was included in the object");}
	newTransparent->ShaderInput.FileName = iter->second;
	newTransparent->pInputLayout.first = iter->second;

	iter = info.find("ShaderInputEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Input Entry point Name was included in the object");}
	newTransparent->ShaderInput.EntryPoint = iter->second;

	iter = info.find("ShaderInputModel");
	if(iter == info.end()){throw std::exception("No Shader Input Model point Name was included in the object");}
	newTransparent->ShaderInput.Mode = iter->second;

	// Shader Vertex
	iter = info.find("ShaderVertexFileName");
	if(iter == info.end()){throw std::exception("No Shader Vertex File Name was included in the object");}
	newTransparent->ShaderVertex.FileName = iter->second;	
	newTransparent->pVertexShader.first = iter->second;

	iter = info.find("ShaderVertexEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Vertex Entry point Name was included in the object");}
	newTransparent->ShaderVertex.EntryPoint = iter->second;

	iter = info.find("ShaderVertexModel");
	if(iter == info.end()){throw std::exception("No Shader Vertex Model point Name was included in the object");}
	newTransparent->ShaderVertex.Mode = iter->second;

	// Shader Pixel
	iter = info.find("ShaderPixelFileName");
	if(iter == info.end()){throw std::exception("No Shader Pixel File Name was included in the object");}
	newTransparent->ShaderPixel.FileName = iter->second;
	newTransparent->pPixelShader.first = iter->second;

	iter = info.find("ShaderPixelEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Pixel Entry point Name was included in the object");}
	newTransparent->ShaderPixel.EntryPoint = iter->second;

	iter = info.find("ShaderPixelModel");
	if(iter == info.end()){throw std::exception("No Shader Pixel Model point Name was included in the object");}
	newTransparent->ShaderPixel.Mode = iter->second;

	// Scale XYZ
	iter = info.find("XYZScaleX");
	if(iter != info.end()) { newTransparent->object.Scale.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZScaleY");
	if(iter != info.end()) { newTransparent->object.Scale.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZScaleZ");
	if(iter != info.end()) { newTransparent->object.Scale.z = Helper::StringToFloat(iter->second); } 
	
	// Location
	iter = info.find("XYZLocationX");
	if(iter != info.end()) { newTransparent->object.Pos.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZLocationY");
	if(iter != info.end()) { newTransparent->object.Pos.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZLocationZ");
	if(iter != info.end()) { newTransparent->object.Pos.z = Helper::StringToFloat(iter->second); } 

	// XYZRotation
	iter = info.find("XYZRotationX");
	if(iter != info.end()) { newTransparent->object.Rot.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZRotationY");
	if(iter != info.end()) { newTransparent->object.Rot.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZRotationZ");
	if(iter != info.end()) { newTransparent->object.Rot.z = Helper::StringToFloat(iter->second); } 

	// XYZOrbit
	iter = info.find("XYZOrbitX");
	if(iter != info.end()) { newTransparent->object.Orbit.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZOrbitY");
	if(iter != info.end()) { newTransparent->object.Orbit.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZOrbitZ");
	if(iter != info.end()) { newTransparent->object.Orbit.z = Helper::StringToFloat(iter->second); } 

	// XYZDiffuse
	iter = info.find("XYZDiffuseX");
	if(iter != info.end()) { newTransparent->object.Colour.Diffuse.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZDiffuseY");
	if(iter != info.end()) { newTransparent->object.Colour.Diffuse.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZDiffuseZ");
	if(iter != info.end()) { newTransparent->object.Colour.Diffuse.z = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZDiffuseW");
	if(iter != info.end()) { newTransparent->object.Colour.Diffuse.w = Helper::StringToFloat(iter->second); } 

	// XYZAmbent
	iter = info.find("XYZAmbentX");
	if(iter != info.end()) { newTransparent->object.Colour.Ambient.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZAmbentY");
	if(iter != info.end()) { newTransparent->object.Colour.Ambient.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZAmbentZ");
	if(iter != info.end()) { newTransparent->object.Colour.Ambient.z = Helper::StringToFloat(iter->second); } 

	// XYZAmbent
	iter = info.find("XYZSpecX");
	if(iter != info.end()) { newTransparent->object.Colour.Spec.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZSpecY");
	if(iter != info.end()) { newTransparent->object.Colour.Spec.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZSpecZ");
	if(iter != info.end()) { newTransparent->object.Colour.Spec.z = Helper::StringToFloat(iter->second); } 


	for(int counter = 0; true; ++counter)
	{
		std::stringstream ss;
		ss << "Texture" << counter;
		iter = info.find(ss.str());

		if(iter == info.end())
		{
			break;
		}
		else
		{
			newTransparent->pVecTexture.emplace_back(std::make_pair<std::string, ID3D11ShaderResourceView*>(iter->second, NULL));
		}
	}

	iter = info.find("CubeMap");
	if(iter != info.end()) { newTransparent->pCubeMap.first = iter->second; } 

	return newTransparent;
}