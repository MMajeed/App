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
	this->VertexBufferID        = Helper::WStringtoString( plyFileName);
	this->IndexBufferID         = Helper::WStringtoString( plyFileName);
	this->InputLayoutID         = "ShaderFiles/0_Basic.fx";
	this->VertexShaderID        = "ShaderFiles/0_Basic.fx";
	this->PixelShaderID         = "ShaderFiles/0_Basic.fx";
	this->RastersizerStateID    = "RastersizerWireFrame";	
	this->CBChangesEveryFrameID = "CBChangeEveryFrame";

	std::wstring error;
	
	bool hr = VertexBuffer::LoadFromPlyFile(plyFileName, this->vertexBuffer,error); 
	if(!hr)
	{
		throw std::exception("Error loading ply file");
	}	
}