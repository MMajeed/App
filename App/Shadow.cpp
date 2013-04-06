#include "Shadow.h"
#include "Application.h"

bool						Shadow::IsInited		= false;
ID3D11ShaderResourceView*	Shadow::pDepthMapSRV	= NULL;
ID3D11DepthStencilView*		Shadow::pDepthMapDSV	= NULL;


void Shadow::Init()
{
	Shadow::BuildDepthMap();
}

void Shadow::CreateShadow()
{
	if(Shadow::IsInited == false)
	{
		Shadow::Init();
		Shadow::IsInited = true;
	}
/*
	auto d3dStuff = DX11App::getInstance()->direct3d;

	ID3D11RenderTargetView* renderTargets[1] = {Shadow::pColorMapRTV};
	d3dStuff.pImmediateContext->OMSetRenderTargets(1, renderTargets, Shadow::pDepthMapDSV);

	float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	d3dStuff.pImmediateContext->ClearRenderTargetView(Shadow::pColorMapRTV, black);

	d3dStuff.pImmediateContext->ClearDepthStencilView(Shadow::pDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	Application::getInstance()->DrawObjects();

	d3dStuff.pImmediateContext->GenerateMips(Shadow::pDepthMapSRV);

	d3dStuff.pImmediateContext->OMSetRenderTargets(1, &(d3dStuff.pRenderTargetView), d3dStuff.pDepthStencilView);	

	d3dStuff.pImmediateContext->PSSetShaderResources( 0, 1, &(Shadow::pDepthMapSRV) );*/
}

void Shadow::BuildDepthMap()
{
	//auto d3dStuff = DX11App::getInstance()->direct3d;

	//ID3D11Texture2D* depthMap = 0;
	//D3D11_TEXTURE2D_DESC texDesc;

	//texDesc.Width              = App::getInstance()->window.width;
	//texDesc.Height             = App::getInstance()->window.height;
	//texDesc.MipLevels          = 1;
	//texDesc.ArraySize          = 1;
	//texDesc.Format             = DXGI_FORMAT_R32_TYPELESS;
	//texDesc.SampleDesc.Count   = 1;
	//texDesc.SampleDesc.Quality = 0;
	//texDesc.Usage              = D3D11_USAGE_DEFAULT;
	//texDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE ;
	//texDesc.CPUAccessFlags     = 0;
	//texDesc.MiscFlags          = 0;

	//HRESULT hr = d3dStuff.pd3dDevice->CreateTexture2D(&texDesc, 0, &depthMap);
	//if(hr)
	//{
	//	throw std::exception("Failed at creating the texture 2d for the Sniper");
	//}

	//D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;

	//ZeroMemory(&dsvDesc, sizeof(dsvDesc));
 //   dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
 //   dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
 //   dsvDesc.Texture2D.MipSlice = 0;
	//hr = d3dStuff.pd3dDevice->CreateDepthStencilView(depthMap,&dsvDesc, &(Shadow::pDepthMapDSV));
	//if(hr)
	//{
	//	throw std::exception("Failed at creating the Depth Stencil for sniper");
	//}

 //   D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
 //   srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
 //   srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
 //   srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
 //   srvDesc.Texture2D.MostDetailedMip = 0;
	//hr = d3dStuff.pd3dDevice->CreateShaderResourceView(depthMap, &srvDesc, &(Shadow::pDepthMapSRV));
	//if(hr)
	//{
	//	throw std::exception("Failed at creating a shader resource view for sniper");
	//}

	//// Null description means to create a view to all mipmap levels
 //   // using the format the texture was created with.
	//hr = d3dStuff.pd3dDevice->CreateRenderTargetView(depthMap, 0, &(Shadow::pColorMapRTV));
 //   if(hr)
	//{
	//	throw std::exception("Failed at creating render target view");
	//}

 //   // View saves a reference to the texture so we can
 //   // release our reference.
	//depthMap->Release();
}