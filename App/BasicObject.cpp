#include "BasicObject.h"
#include "DX11ObjectManager.h"
#include "App.h"
#include "DX11App.h"
#include "DX11Helper.h"
#include "Helper.h"
#include "cBuffer.h"
#include "VertexBuffer.h"
#include "Helper.h"
#include <sstream>

BasicObject::BasicObject( )
{		
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
void BasicObject::Update(float delta)
{
	this->object.Update(delta);
}
void BasicObject::Draw()
{
	cBuffer::cbChangeEveryFrame cbCEF ;
	XMFLOAT4X4 world = this->object.CalculateMatrix();
	cbCEF.mWorld = XMLoadFloat4x4(&world);
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
		
	this->DrawTexture(pImmediateContext);

	// Set vertex buffer 
	UINT stride = sizeof( VertexBuffer::SimpleVertex );
	UINT offset = 0;
	pImmediateContext->UpdateSubresource( this->pCBChangesEveryFrame.second, 0, NULL, &cbCEF, 0, 0 );
	pImmediateContext->IASetVertexBuffers( 0, 1, &this->pVertexBuffer.second, &stride, &offset );
	pImmediateContext->IASetIndexBuffer( this->pIndexBuffer.second, DXGI_FORMAT_R16_UINT, 0 );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Set the input layout
	pImmediateContext->IASetInputLayout( this->pInputLayout.second );
	pImmediateContext->VSSetShader( this->pVertexShader.second, NULL, 0 );	
	pImmediateContext->PSSetShader( this->pPixelShader.second, NULL, 0 );
	pImmediateContext->RSSetState(this->pRastersizerState.second);
	pImmediateContext->VSSetConstantBuffers( 2, 1, &(this->pCBChangesEveryFrame.second) );
	pImmediateContext->DrawIndexed( this->vertexBuffer.indices.size() * 3, 0, 0 );
}
void BasicObject::LoadD3DStuff()
{
	if(!DX11ObjectManager::getInstance()->VertexBuffer.Get(this->pVertexBuffer.first, this->pVertexBuffer.second)){ throw std::exception("Vertex Buffer not found"); }
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

void BasicObject::DrawTexture(ID3D11DeviceContext* pImmediateContext)
{
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

void BasicObject::InitVertexBuffer(ID3D11Device* device)
{
	std::wstring error;

	if(!DX11ObjectManager::getInstance()->VertexBuffer.Exists(this->pVertexBuffer.first))
	{
		if(!this->vertexBuffer.CreateVertexBuffer(device, &(this->pVertexBuffer.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}	
		DX11ObjectManager::getInstance()->VertexBuffer.Add(this->pVertexBuffer.first, pVertexBuffer.second);
	}
}
void BasicObject::InitIndexBuffer(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->IndexBuffer.Exists(this->pIndexBuffer.first))
	{
		std::wstring error;
		if(!this->vertexBuffer.CreateIndexBuffer(device, &(this->pIndexBuffer.second), error))
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
		if(!DX11Helper::LoadInputLayoutFile(ShaderInput.FileName, ShaderInput.EntryPoint, ShaderInput.Mode, device, &(this->pInputLayout.second), error))
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
		if(!DX11Helper::LoadVertexShaderFile(ShaderVertex.FileName, ShaderVertex.EntryPoint, ShaderVertex.Mode, device, &(this->pVertexShader.second), error))
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
		if(!DX11Helper::LoadPixelShaderFile(ShaderPixel.FileName, ShaderPixel.EntryPoint, ShaderPixel.Mode, device, &(this->pPixelShader.second), error))
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