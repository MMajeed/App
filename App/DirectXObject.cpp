#include "DirectXObject.h"
#include "Application.h"
#include "DX11ObjectManager.h"
#include "Helper.h"

void DirectXObject::Init()
{
	ID3D11Device* pDevice = DX11App::getInstance()->direct3d.pd3dDevice;

	this->InitDepthRastersizerState(pDevice);
}

void DirectXObject::DrawDepth()
{

	this->SetupDrawConstantBuffer();
	this->SetupDrawVertexBuffer();
	this->SetupDrawInputVertexShader();
	this->SetupDrawPixelShader();
	if(this->pDepthRasterierState.second != NULL)
	{
		ID3D11DeviceContext* pImmediateContext = DX11App::getInstance()->direct3d.pImmediateContext;

		pImmediateContext->RSSetState(this->pDepthRasterierState.second);
	}
	else
	{
		this->SetupDrawRasterizeShader();
	}
	this->SetupDrawTexture();
	this->DrawObject();
	this->CleanupAfterDraw();
}

void DirectXObject::InitDepthRastersizerState(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->RastersizerState.Exists(this->pDepthRasterierState.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadRasterizerState(D3D11_CULL_FRONT, D3D11_FILL_SOLID, true, true, device, &(this->pDepthRasterierState.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->RastersizerState.Add(this->pDepthRasterierState.first, this->pDepthRasterierState.second);
	}
	if(!DX11ObjectManager::getInstance()->RastersizerState.Get(this->pDepthRasterierState.first, this->pDepthRasterierState.second)){ throw std::exception("Depth Rastersizer State not found"); }
}

DirectXObject::DirectXObject()
{
	this->pDepthRasterierState.first = "DethCullFrontFillSolid";
	this->pDepthRasterierState.second = NULL;

}



