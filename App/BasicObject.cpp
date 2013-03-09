#include "BasicObject.h"
#include "DX11ObjectManager.h"
#include "App.h"
#include "DX11App.h"
#include "DX11Helper.h"
#include "Helper.h"
#include "cBuffer.h"
#include "PlyBuffer.h"
#include "Helper.h"
#include <sstream>

BasicObject::BasicObject( )
{	
	this->pCBChangesEveryFrame.first = "ChangesEveryFrame";
	this->pRastersizerState.first = "FillSolid";
}
void BasicObject::Init()
{
	ID3D11Device* device = (dynamic_cast<DX11App*>(App::getInstance()))->direct3d.pd3dDevice;

	this->InitVertexBuffer(device);
	this->InitIndexBuffer(device);
	this->InitInputLayout(device);
	this->InitVertexShader(device);
	this->InitPixelShader(device);
	this->InitRastersizerState(device);
	this->InitCBChangesEveryFrameBuffer(device);
	this->InitTextureAndCube(device);

	this->LoadD3DStuff();
}
void BasicObject::Clean()
{
}
void BasicObject::UpdateDrawing(float delta)
{
	UNREFERENCED_PARAMETER(delta);
}
void BasicObject::UpdateObject(float delta)
{
	this->object.Update(delta);
}
void BasicObject::Draw()
{
	this->SetupDraw();
	this->SetupTexture();
	this->DrawObject();
	this->CleanupAfterDraw();
}
void BasicObject::SetupDraw()
{
	cBuffer::cbChangeEveryFrame cbCEF ;
	XMFLOAT4X4 world = this->object.CalculateMatrix();
	cbCEF.mWorld = XMLoadFloat4x4(&world);
	cbCEF.colour.diffuse = this->object.Colour.Diffuse;
	cbCEF.colour.ambient = this->object.Colour.Ambient;
	cbCEF.colour.spec = this->object.Colour.Spec;

	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;

	pImmediateContext->UpdateSubresource( this->pCBChangesEveryFrame.second, 0, NULL, &cbCEF, 0, 0 );
	pImmediateContext->VSSetConstantBuffers( 2, 1, &(this->pCBChangesEveryFrame.second) );
	pImmediateContext->PSSetConstantBuffers( 2, 1, &(this->pCBChangesEveryFrame.second) );
		
	// Set vertex buffer 
	UINT stride = sizeof( PlyBuffer::Vertex );
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers( 0, 1, &this->pVertexBuffer.second, &stride, &offset );
	pImmediateContext->IASetIndexBuffer( this->pIndexBuffer.second, DXGI_FORMAT_R16_UINT, 0 );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Set the input layout
	pImmediateContext->IASetInputLayout( this->pInputLayout.second );
	pImmediateContext->VSSetShader( this->pVertexShader.second, NULL, 0 );	
	pImmediateContext->PSSetShader( this->pPixelShader.second, NULL, 0 );

	pImmediateContext->RSSetState(this->pRastersizerState.second);
}
void BasicObject::SetupTexture()
{
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
	
	int counter = 0;
	for(auto textureIter = pVecTexture.begin();
		textureIter != pVecTexture.end();
		++textureIter)
	{
		pImmediateContext->PSSetShaderResources( counter, 1, &(textureIter->second) );
		++counter;
	}

	if(this->pCubeMap.first != "")
	{
		pImmediateContext->PSSetShaderResources( counter, 1, &this->pCubeMap.second );	
	}
}
void BasicObject::DrawObject()
{
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
	
	pImmediateContext->DrawIndexed( this->PlyBuffer.indices.size(), 0, 0 );
}
void BasicObject::CleanupAfterDraw()
{
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
	
	ID3D11ShaderResourceView* tab = NULL;
	pImmediateContext->PSSetShaderResources(0,1,&tab);
	pImmediateContext->PSSetShaderResources(1,1,&tab);
	pImmediateContext->PSSetShaderResources(2,1,&tab);
}
void BasicObject::LoadD3DStuff()
{
	if(!DX11ObjectManager::getInstance()->PlyBuffer.Get(this->pVertexBuffer.first, this->pVertexBuffer.second)){ throw std::exception("Vertex Buffer not found"); }
	if(!DX11ObjectManager::getInstance()->IndexBuffer.Get(this->pIndexBuffer.first, this->pIndexBuffer.second)){ throw std::exception("Index Buffer not found"); }
	if(!DX11ObjectManager::getInstance()->InputLayout.Get(this->pInputLayout.first, this->pInputLayout.second)){ throw std::exception("Input Layout not found"); }
	if(!DX11ObjectManager::getInstance()->VertexShader.Get(this->pVertexShader.first, this->pVertexShader.second)){ throw std::exception("Vertex Shader not found"); }
	if(!DX11ObjectManager::getInstance()->PixelShader.Get(this->pPixelShader.first, this->pPixelShader.second)){ throw std::exception("Pixel Shader not found"); }
	if(!DX11ObjectManager::getInstance()->RastersizerState.Get(this->pRastersizerState.first, this->pRastersizerState.second)){ throw std::exception("Rastersizer State not found"); }
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->pCBChangesEveryFrame.first, this->pCBChangesEveryFrame.second)){ throw std::exception("const buffer not found"); }	

	for(auto iter = pVecTexture.begin();
		iter != pVecTexture.end();
		++iter)
	{
		if(!DX11ObjectManager::getInstance()->Textexture.Get(iter->first, iter->second)){ throw std::exception("Texture not found"); }	
	}

	if(this->pCubeMap.first != "")
	{
		if(!DX11ObjectManager::getInstance()->Textexture.Get(this->pCubeMap.first, this->pCubeMap.second)){ throw std::exception("Cube Map not found"); }	
	}
}
float BasicObject::GetOrder()
{
	return 1000.0f;
}

void BasicObject::InitVertexBuffer(ID3D11Device* device)
{
	std::wstring error;

	if(!DX11ObjectManager::getInstance()->PlyBuffer.Exists(this->pVertexBuffer.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadVertexBuffer<PlyBuffer::Vertex>(device, &(this->PlyBuffer.vertices.front()), this->PlyBuffer.vertices.size(), &(this->pVertexBuffer.second), error ))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->PlyBuffer.Add(this->pVertexBuffer.first, pVertexBuffer.second);
	}
}
void BasicObject::InitIndexBuffer(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->IndexBuffer.Exists(this->pIndexBuffer.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadIndexBuffer<WORD>(device, &(this->PlyBuffer.indices.front()), this->PlyBuffer.indices.size(), &(this->pIndexBuffer.second), error ))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->IndexBuffer.Add(this->pIndexBuffer.first, this->pIndexBuffer.second);
	}
}
void BasicObject::InitInputLayout(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->InputLayout.Exists(this->pInputLayout.first))
	{
		std::wstring error;
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT numElements = ARRAYSIZE( layout );

		if(!DX11Helper::LoadInputLayoutFile(Shader.ShaderInput.FileName, Shader.ShaderInput.EntryPoint, Shader.ShaderInput.Mode, device, layout, numElements, &(this->pInputLayout.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->InputLayout.Add(this->pInputLayout.first, this->pInputLayout.second);
	}
}
void BasicObject::InitVertexShader(ID3D11Device* device)
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
void BasicObject::InitPixelShader(ID3D11Device* device)
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
void BasicObject::InitRastersizerState(ID3D11Device* device)
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
void BasicObject::InitCBChangesEveryFrameBuffer(ID3D11Device* device)
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
void BasicObject::InitTextureAndCube(ID3D11Device* device)
{
	for(auto textureIter = this->pVecTexture.begin();
		textureIter != this->pVecTexture.end();
		++textureIter)
	{
		std::string textureName = textureIter->first;
		if(!DX11ObjectManager::getInstance()->Textexture.Exists(textureName))
		{
			std::wstring error;
			if(!DX11Helper::LoadTextureFile(Helper::stringToWstring(textureName), device, &(textureIter->second), error))
			{
				throw std::exception(Helper::WStringtoString(error).c_str());
			}
			DX11ObjectManager::getInstance()->Textexture.Add(textureName, textureIter->second);
		}
	}

	if(this->pCubeMap.first != "")
	{
		std::string textureName = this->pCubeMap.first;
		if(!DX11ObjectManager::getInstance()->Textexture.Exists(textureName))
		{
			std::wstring error;
			if(!DX11Helper::LoadTextureFile(Helper::stringToWstring(textureName), device, &(this->pCubeMap.second), error))
			{
				throw std::exception(Helper::WStringtoString(error).c_str());
			}
			DX11ObjectManager::getInstance()->Textexture.Add(textureName, this->pCubeMap.second);
		}
	}
}