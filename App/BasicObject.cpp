#include "BasicObject.h"
#include "DX11ObjectManager.h"
#include "App.h"
#include "DX11App.h"
#include "DX11Helper.h"
#include "Helper.h"
#include "cBuffer.h"
#include "VertexBuffer.h"
#include "Helper.h"

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
}
void BasicObject::Clean()
{
	DX11ObjectManager::getInstance()->VertexBuffer.Erase(this->VertexBufferID);
	DX11ObjectManager::getInstance()->VertexBuffer.Erase(this->IndexBufferID);
	DX11ObjectManager::getInstance()->InputLayout.Erase(this->InputLayoutID );
	DX11ObjectManager::getInstance()->VertexShader.Erase(this->VertexShaderID);
	DX11ObjectManager::getInstance()->PixelShader.Erase(this->PixelShaderID);
	DX11ObjectManager::getInstance()->RastersizerState.Erase(this->RastersizerStateID);
	DX11ObjectManager::getInstance()->CBuffer.Erase(this->CBChangesEveryFrameID);
}
void BasicObject::Update(float delta)
{
	this->object.Update(delta);
}
void BasicObject::Draw()
{
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;
	ID3D11InputLayout* pInputLayout;
	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;
	ID3D11RasterizerState* pRastersizerState;
	ID3D11Buffer* pCBbChangeEveryFrame;
	this->LoadD3DStuff(pVertexBuffer, pIndexBuffer, pInputLayout, pVertexShader, pPixelShader, pRastersizerState, pCBbChangeEveryFrame);

	cBuffer::cbChangeEveryFrame cbCEF ;
	XMFLOAT4X4 world = this->object.CalculateMatrix();
	cbCEF.mWorld = XMLoadFloat4x4(&world);
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
		
	this->DrawTexture(pImmediateContext);

	// Set vertex buffer 
	UINT stride = sizeof( VertexBuffer::SimpleVertex );
	UINT offset = 0;
	pImmediateContext->UpdateSubresource( pCBbChangeEveryFrame, 0, NULL, &cbCEF, 0, 0 );
	pImmediateContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );
	pImmediateContext->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Set the input layout
	pImmediateContext->IASetInputLayout( pInputLayout );
	pImmediateContext->VSSetShader( pVertexShader, NULL, 0 );	
	pImmediateContext->PSSetShader( pPixelShader, NULL, 0 );
	pImmediateContext->RSSetState(pRastersizerState);
	pImmediateContext->VSSetConstantBuffers( 2, 1, &pCBbChangeEveryFrame );
	pImmediateContext->DrawIndexed( this->vertexBuffer.indices.size() * 3, 0, 0 );
}
void BasicObject::LoadD3DStuff(	ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer, ID3D11InputLayout*& inputLayout, ID3D11VertexShader*& vertexShader, ID3D11PixelShader*& pixelShader, ID3D11RasterizerState*& rastersizerState, ID3D11Buffer*& cbChangeEveryFrame) const
{
	if(!DX11ObjectManager::getInstance()->VertexBuffer.Get(this->VertexBufferID , vertexBuffer)){ throw std::exception("Vertex Buffer not found"); }
	if(!DX11ObjectManager::getInstance()->IndexBuffer.Get(this->IndexBufferID , indexBuffer)){ throw std::exception("Index Buffer not found"); }
	if(!DX11ObjectManager::getInstance()->InputLayout.Get(this->InputLayoutID , inputLayout)){ throw std::exception("Input Layout not found"); }
	if(!DX11ObjectManager::getInstance()->VertexShader.Get(this->VertexShaderID , vertexShader)){ throw std::exception("Vertex Shader not found"); }
	if(!DX11ObjectManager::getInstance()->PixelShader.Get(this->PixelShaderID , pixelShader)){ throw std::exception("Pixel Shader not found"); }
	if(!DX11ObjectManager::getInstance()->RastersizerState.Get(this->RastersizerStateID , rastersizerState)){ throw std::exception("Rastersizer State not found"); }
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->CBChangesEveryFrameID , cbChangeEveryFrame)){ throw std::exception("const buffer not found"); }	
}

void BasicObject::DrawTexture(ID3D11DeviceContext* pImmediateContext)
{
	std::vector<ID3D11ShaderResourceView*> textures;
	for(auto textureIter = this->TextureIDs.begin();
		textureIter != this->TextureIDs.end();
		++textureIter)
	{
		ID3D11ShaderResourceView* tempTexture;

		if(!DX11ObjectManager::getInstance()->Textexture.Get(*textureIter, tempTexture)){ throw std::exception(("Texture " + *textureIter + " not found").c_str()); }

		textures.push_back(tempTexture);
	}

	int counter = 0;

	for(auto textureIter = textures.begin();
		textureIter != textures.end();
		++textureIter)
	{
		pImmediateContext->PSSetShaderResources( counter, 1, &(*textureIter) );
		++counter;
	}

	if(this->CubeMapIDs != "")
	{
		ID3D11ShaderResourceView* cubeMap;
		if(!DX11ObjectManager::getInstance()->Textexture.Get(this->CubeMapIDs, cubeMap)){ throw std::exception(("Cubemap " + this->CubeMapIDs + " not found").c_str()); }
		pImmediateContext->PSSetShaderResources( counter, 1, &cubeMap );	
	}
}

void BasicObject::InitVertexBuffer(ID3D11Device* device)
{
	std::wstring error;

	if(!DX11ObjectManager::getInstance()->VertexBuffer.Exists(this->VertexBufferID))
	{
		ID3D11Buffer* vertexBuffer;
		if(!this->vertexBuffer.CreateVertexBuffer(device, &vertexBuffer, error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}	
		DX11ObjectManager::getInstance()->VertexBuffer.Add(this->VertexBufferID, vertexBuffer);
	}
}
void BasicObject::InitIndexBuffer(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->IndexBuffer.Exists(this->IndexBufferID))
	{
		std::wstring error;
		ID3D11Buffer* indexBuffer;
		if(!this->vertexBuffer.CreateIndexBuffer(device, &indexBuffer, error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->IndexBuffer.Add(this->IndexBufferID, indexBuffer);
	}
}
void BasicObject::InitInputLayout(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->InputLayout.Exists(this->InputLayoutID))
	{
		std::wstring error;
		ID3D11InputLayout* inputLayout;
		if(!DX11Helper::LoadInputLayoutFile(Helper::stringToWstring(this->InputLayoutID), L"VS", L"vs_4_0", device, &inputLayout, error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->InputLayout.Add(this->InputLayoutID, inputLayout);
	}
}
void BasicObject::InitVertexShader(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->VertexShader.Exists(this->VertexShaderID))
	{
		std::wstring error;
		ID3D11VertexShader* vertexShader;
		if(!DX11Helper::LoadVertexShaderFile(Helper::stringToWstring(this->VertexShaderID), L"VS", L"vs_4_0", device, &vertexShader, error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->VertexShader.Add(this->VertexShaderID , vertexShader);
	}
}
void BasicObject::InitPixelShader(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->PixelShader.Exists(this->PixelShaderID))
	{
		std::wstring error;
		ID3D11PixelShader* pixelShader;
		if(!DX11Helper::LoadPixelShaderFile(Helper::stringToWstring(this->PixelShaderID), L"PS", L"ps_4_0", device, &pixelShader, error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->PixelShader.Add(this->PixelShaderID , pixelShader);
	}
}
void BasicObject::InitRastersizerState(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->RastersizerState.Exists(this->RastersizerStateID))
	{
		std::wstring error;
		ID3D11RasterizerState* rasterizerState;
		if(!DX11Helper::LoadRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, true, true, device, &rasterizerState, error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->RastersizerState.Add(this->RastersizerStateID, rasterizerState);
	}
}
void BasicObject::InitCBChangesEveryFrameBuffer(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->CBuffer.Exists(this->CBChangesEveryFrameID))
	{
		std::wstring error;
		ID3D11Buffer* constantBuffer;
		if(!cBuffer::LoadBuffer<cBuffer::cbChangeEveryFrame>(device, &constantBuffer, error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->CBuffer.Add(this->CBChangesEveryFrameID, constantBuffer);
	}
}
void BasicObject::InitTextureAndCube(ID3D11Device* device)
{
	for(auto textureIter = this->TextureIDs.begin();
		textureIter != this->TextureIDs.end();
		++textureIter)
	{
		std::string textureName = *textureIter;
		if(!DX11ObjectManager::getInstance()->Textexture.Exists(textureName))
		{
			std::wstring error;
			ID3D11ShaderResourceView* texture;
			if(!DX11Helper::LoadTextureFile(Helper::stringToWstring(textureName), device, &texture, error))
			{
				throw std::exception(Helper::WStringtoString(error).c_str());
			}
			DX11ObjectManager::getInstance()->Textexture.Add(textureName, texture);
		}
	}

	if(this->CubeMapIDs != "")
	{
		std::string textureName = this->CubeMapIDs;
		if(!DX11ObjectManager::getInstance()->Textexture.Exists(textureName))
		{
			std::wstring error;
			ID3D11ShaderResourceView* texture;
			if(!DX11Helper::LoadTextureFile(Helper::stringToWstring(textureName), device, &texture, error))
			{
				throw std::exception(Helper::WStringtoString(error).c_str());
			}
			DX11ObjectManager::getInstance()->Textexture.Add(textureName, texture);
		}
	}
}