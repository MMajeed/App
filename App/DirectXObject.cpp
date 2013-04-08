#include "DirectXObject.h"
#include "Application.h"
#include "DX11ObjectManager.h"
#include "Helper.h"

void DirectXObject::InitDepth()
{
	this->pDepthRastersizerState.first = "CullFrontFill";
	this->pDepthPixelShader.first = "../Resources/ShaderFiles/8_Depth.fx";
	ID3D11Device* device = DX11App::getInstance()->direct3d.pd3dDevice;
	DirectXObject::InitDepthPixelShader(device);
	DirectXObject::InitDepthRastersizerState(device);
}

void DirectXObject::DrawDepth()
{
	ID3D11DeviceContext* pImmediateContext = DX11App::getInstance()->direct3d.pImmediateContext;

	this->SetupDrawConstantBuffer();
	this->SetupDrawVertexBuffer();
	this->SetupDrawInputVertexShader();

	pImmediateContext->PSSetShader( this->pDepthPixelShader.second, NULL, 0 );
	pImmediateContext->RSSetState(this->pDepthRastersizerState.second);

	this->SetupDrawTexture();
	this->DrawObject();
	this->CleanupAfterDraw();
}

void DirectXObject::InitDepthPixelShader(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->PixelShader.Exists(this->pDepthPixelShader.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadPixelShaderFile(this->pDepthPixelShader.first, "PS", "ps_4_0", device, &(this->pDepthPixelShader.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->PixelShader.Add(this->pDepthPixelShader.first , this->pDepthPixelShader.second);
	}

	if(!DX11ObjectManager::getInstance()->PixelShader.Get(this->pDepthPixelShader.first, this->pDepthPixelShader.second)){ throw std::exception("Failed to load depth pixel shader"); }
}
void DirectXObject::InitDepthRastersizerState(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->RastersizerState.Exists(this->pDepthRastersizerState.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadRasterizerState(D3D11_CULL_FRONT, D3D11_FILL_SOLID, true, true, device, &(this->pDepthRastersizerState.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->RastersizerState.Add(this->pDepthRastersizerState.first, this->pDepthRastersizerState.second);
	}

	if(!DX11ObjectManager::getInstance()->RastersizerState.Get(this->pDepthRastersizerState.first, this->pDepthRastersizerState.second)){ throw std::exception("Failed to load depth rasterizer state"); }
}

DirectXObject::DirectXObject()
{
	BoolDrawDepth = true;
}