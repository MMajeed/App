#include "BasicObject.h"
#include "DX11ObjectManager.h"
#include "App.h"
#include "DX11App.h"
#include "DX11Helper.h"
#include "Helper.h"
#include "cBuffer.h"
#include "VertexBuffer.h"
#include "PlyFile.h"

PlyFile::PlyFile(std::wstring plyFileName)
{	
	this->pVertexBuffer.first        = Helper::WStringtoString( plyFileName);
	this->pIndexBuffer.first         = Helper::WStringtoString( plyFileName);
	this->pCBChangesEveryFrame.first = "CBChangeEveryFrame";
	this->pInputLayout.first         = "ShaderFiles/0_Basic.fx";
	this->pVertexShader.first        = "ShaderFiles/0_Basic.fx";
	this->pPixelShader.first         = "ShaderFiles/0_Basic.fx";
	this->pRastersizerState.first    = "RastersizerWireFrame";	

	std::wstring error;
	
	bool hr = VertexBuffer::LoadFromPlyFile(plyFileName, this->vertexBuffer,error); 
	if(!hr)
	{
		throw std::exception("Error loading ply file");
	}	
}