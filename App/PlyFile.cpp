#include "BasicObject.h"
#include "DX11ObjectManager.h"
#include "App.h"
#include "DX11App.h"
#include "DX11Helper.h"
#include "Helper.h"
#include "cBuffer.h"
#include "VertexBuffer.h"
#include "PlyFile.h"
#include <sstream>

PlyFile::PlyFile()
{

}

PlyFile::PlyFile(std::wstring plyFileName)
{	
	this->pVertexBuffer.first        = Helper::WStringtoString( plyFileName);
	this->pIndexBuffer.first         = Helper::WStringtoString( plyFileName);
	this->pCBChangesEveryFrame.first = "CBChangeEveryFrame";
	this->pInputLayout.first         = "ShaderFiles/0_Basic.fx";
	this->pVertexShader.first        = "ShaderFiles/0_Basic.fx";
	this->pPixelShader.first         = "ShaderFiles/0_Basic.fx";
	this->pRastersizerState.first    = "RastersizerWireFrame";	

	this->ShaderInput.FileName		= "ShaderFiles/0_Basic.fx";
	this->ShaderInput.EntryPoint	= "VS";	
	this->ShaderInput.Mode			= "vs_4_0";

	this->ShaderVertex.FileName		= "ShaderFiles/0_Basic.fx";
	this->ShaderVertex.EntryPoint	= "VS";	
	this->ShaderVertex.Mode			= "vs_4_0";

	this->ShaderPixel.FileName		= "ShaderFiles/0_Basic.fx";
	this->ShaderPixel.EntryPoint	= "PS";	
	this->ShaderPixel.Mode			= "ps_4_0";


	std::wstring error;
	
	bool hr = VertexBuffer::LoadFromPlyFile(plyFileName, this->vertexBuffer,error); 
	if(!hr)
	{
		throw std::exception("Error loading ply file");
	}	
}

PlyFile* PlyFile::Spawn(std::map<std::string, std::string> info)
{
	PlyFile* newPlyFile = new PlyFile;
	
	std::wstring error;

	auto iter = info.find("PlyFile");
	if(iter == info.end()){throw std::exception("No ply file was included in the object");}
	bool hr = VertexBuffer::LoadFromPlyFile(Helper::stringToWstring(iter->second), newPlyFile->vertexBuffer, error); 
	if(!hr){ throw std::exception("Error loading ply file"); }	
	newPlyFile->pVertexBuffer.first = iter->second;
	newPlyFile->pIndexBuffer.first = iter->second;

	// Shader Input
	iter = info.find("ShaderInputFileName");
	if(iter == info.end()){throw std::exception("No Shader Input File Name was included in the object");}
	newPlyFile->ShaderInput.FileName = iter->second;
	newPlyFile->pInputLayout.first = iter->second;

	iter = info.find("ShaderInputEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Input Entry point Name was included in the object");}
	newPlyFile->ShaderInput.EntryPoint = iter->second;

	iter = info.find("ShaderInputModel");
	if(iter == info.end()){throw std::exception("No Shader Input Model point Name was included in the object");}
	newPlyFile->ShaderInput.Mode = iter->second;

	// Shader Vertex
	iter = info.find("ShaderVertexFileName");
	if(iter == info.end()){throw std::exception("No Shader Vertex File Name was included in the object");}
	newPlyFile->ShaderVertex.FileName = iter->second;	
	newPlyFile->pVertexShader.first = iter->second;

	iter = info.find("ShaderVertexEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Vertex Entry point Name was included in the object");}
	newPlyFile->ShaderVertex.EntryPoint = iter->second;

	iter = info.find("ShaderVertexModel");
	if(iter == info.end()){throw std::exception("No Shader Vertex Model point Name was included in the object");}
	newPlyFile->ShaderVertex.Mode = iter->second;

	// Shader Pixel
	iter = info.find("ShaderPixelFileName");
	if(iter == info.end()){throw std::exception("No Shader Pixel File Name was included in the object");}
	newPlyFile->ShaderPixel.FileName = iter->second;
	newPlyFile->pPixelShader.first = iter->second;

	iter = info.find("ShaderPixelEntryPoint");
	if(iter == info.end()){throw std::exception("No Shader Pixel Entry point Name was included in the object");}
	newPlyFile->ShaderPixel.EntryPoint = iter->second;

	iter = info.find("ShaderPixelModel");
	if(iter == info.end()){throw std::exception("No Shader Pixel Model point Name was included in the object");}
	newPlyFile->ShaderPixel.Mode = iter->second;

	// Scale XYZ
	iter = info.find("XYZScaleX");
	if(iter != info.end()) { newPlyFile->object.Scale.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZScaleY");
	if(iter != info.end()) { newPlyFile->object.Scale.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZScaleZ");
	if(iter != info.end()) { newPlyFile->object.Scale.z = Helper::StringToFloat(iter->second); } 
	
	// Location
	iter = info.find("XYZLocationX");
	if(iter != info.end()) { newPlyFile->object.Pos.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZLocationY");
	if(iter != info.end()) { newPlyFile->object.Pos.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZLocationZ");
	if(iter != info.end()) { newPlyFile->object.Pos.z = Helper::StringToFloat(iter->second); } 

	// XYZRotation
	iter = info.find("XYZRotationX");
	if(iter != info.end()) { newPlyFile->object.Rot.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZRotationY");
	if(iter != info.end()) { newPlyFile->object.Rot.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZRotationZ");
	if(iter != info.end()) { newPlyFile->object.Rot.z = Helper::StringToFloat(iter->second); } 

	// XYZOrbit
	iter = info.find("XYZOrbitX");
	if(iter != info.end()) { newPlyFile->object.Orbit.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZOrbitY");
	if(iter != info.end()) { newPlyFile->object.Orbit.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZOrbitZ");
	if(iter != info.end()) { newPlyFile->object.Orbit.z = Helper::StringToFloat(iter->second); } 


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
			//newPlyFile->pVecTexture.emplace_back(std::make_pair<std::string, ID3D11ShaderResourceView*>(ss.str(), NULL));
		}
	}

	iter = info.find("CubeMap");
	//if(iter != info.end()) { newPlyFile->pCubeMap.first = iter->second; } 

	return newPlyFile;
}