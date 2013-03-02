#include "Sniper.h"
#include "Application.h"
#include "MathHelper.h"
#include "Helper.h"
#include "DX11ObjectManager.h"

void Sniper::BuilDepthMap()
{
	auto d3dStuff = ((DX11App*)App::getInstance())->direct3d;

	ID3D11Texture2D* depthMap = 0;
	D3D11_TEXTURE2D_DESC texDesc;

	texDesc.Width = App::getInstance()->window.width;
	texDesc.Height = App::getInstance()->window.height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HRESULT hr = d3dStuff.pd3dDevice->CreateTexture2D(&texDesc, 0, &depthMap);
	if(hr)
	{
		throw std::exception("Failed at creating the texture 2d for the Sniper");
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;

	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
	hr = d3dStuff.pd3dDevice->CreateDepthStencilView(depthMap,&dsvDesc, &(this->pDepthMapDSV));
	if(hr)
	{
		throw std::exception("Failed at creating the Depth Stencil for sniper");
	}

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
	hr = d3dStuff.pd3dDevice->CreateShaderResourceView(depthMap, &srvDesc, &(this->pDepthMapSRV));
	if(hr)
	{
		throw std::exception("Failed at creating a shader resource view for sniper");
	}

    // View saves a reference to the texture so we can
    // release our reference.
	depthMap->Release();
}
void Sniper::BuildColorMap()
{
	auto d3dStuff = ((DX11App*)App::getInstance())->direct3d;

	ID3D11Texture2D* colorMap = 0;

    D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

    texDesc.Width              = App::getInstance()->window.width;
	texDesc.Height             = App::getInstance()->window.height;
    texDesc.MipLevels          = 0;
    texDesc.ArraySize          = 1;
    texDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count   = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage              = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags     = 0;
    texDesc.MiscFlags          = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	HRESULT hr = d3dStuff.pd3dDevice->CreateTexture2D(&texDesc, 0, &colorMap);
	if(hr)
	{
		throw std::exception("Failed at creating the texture 2d for the Sniper");
	}

	hr = d3dStuff.pd3dDevice->CreateTexture2D(&texDesc, 0, &colorMap);
	if(hr)
	{
		throw std::exception("Failed at creating the texture 2d for the Sniper");
	}

    // Null description means to create a view to all mipmap levels
    // using the format the texture was created with.
	hr = d3dStuff.pd3dDevice->CreateRenderTargetView(colorMap, 0, &(this->pColorMapRTV));
    if(hr)
	{
		throw std::exception("Failed at creating render target view");
	}

	hr = d3dStuff.pd3dDevice->CreateShaderResourceView(colorMap, 0, &(this->pColorMapSRV));
	if(hr)
	{
		throw std::exception("Failed at creating shader resource view");
	}

    // View saves a reference to the texture so we can
    // release our reference.
	colorMap->Release();
}
void Sniper::Init()
{
	BasicObject::Init();
	
	this->BuilDepthMap();
	this->BuildColorMap();

	auto d3dStuff = ((DX11App*)App::getInstance())->direct3d;

	if(!DX11ObjectManager::getInstance()->Textexture.Exists(this->pTextureAlpha.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadTextureFile(Helper::stringToWstring(this->pTextureAlpha.first), d3dStuff.pd3dDevice, &(this->pTextureAlpha.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->Textexture.Add(this->pTextureAlpha.first, this->pTextureAlpha.second);
	}
	if(!DX11ObjectManager::getInstance()->Textexture.Get(this->pTextureAlpha.first, this->pTextureAlpha.second)){ throw std::exception("Texture alpha not found"); }
}


void Sniper::GetNewDynamicTexture()
{
	auto d3dStuff = ((DX11App*)App::getInstance())->direct3d;

	ID3D11RenderTargetView* renderTargets[1] = {this->pColorMapRTV};
	d3dStuff.pImmediateContext->OMSetRenderTargets(1, renderTargets, this->pDepthMapDSV);

	d3dStuff.pImmediateContext->RSSetViewports(1, &(this->pViewport));
	
	float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	d3dStuff.pImmediateContext->ClearRenderTargetView(this->pColorMapRTV, black);

	d3dStuff.pImmediateContext->ClearDepthStencilView(this->pDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	((Application*)App::getInstance())->ClearScreen();
	((Application*)App::getInstance())->DrawObjects();

	d3dStuff.pImmediateContext->GenerateMips(pColorMapSRV);

	d3dStuff.pImmediateContext->OMSetRenderTargets(1, &(d3dStuff.pRenderTargetView), d3dStuff.pDepthStencilView);	
}
void Sniper::UpdateDrawing(float delta)
{
	// Remove this object from the list that we don't want to see	
	std::size_t counter = 0;
	std::vector<iObjectDrawable*> removed;
	std::vector<iObjectDrawable*>& applciationList = ((Application*)App::getInstance())->objects;
	for(counter = 0; counter < applciationList.size(); ++counter)
	{
		if( dynamic_cast<Sniper*>(applciationList[counter]) != 0)
		{
			removed.push_back(applciationList[counter]);
			applciationList.erase(applciationList.begin() + counter);
			--counter;
		}
	}

	// Get the new texture
	this->GetNewDynamicTexture();

	// Put back the ones we removed
	for(auto removedIter = removed.begin();
		removedIter != removed.end();
		++removedIter)
	{
		applciationList.push_back(*removedIter);
	}
}
void Sniper::UpdateObject(float delta)
{
	this->object.Pos = App::getInstance()->camera.Target();

	this->object.Rot.x = App::getInstance()->camera.Pitch() + 1.57f;
	this->object.Rot.y = App::getInstance()->camera.Yaw() ;
	this->object.Rot.z = App::getInstance()->camera.Roll() ;

	this->object.Scale = XMFLOAT4(0.56f, 1.0f, 0.43f, 0.5f);
}

void Sniper::SetupTexture()
{
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;

	if(this->pColorMapSRV != 0)
	{
		pImmediateContext->PSSetShaderResources( 0, 1, &(this->pColorMapSRV) );
	}
	
	pImmediateContext->PSSetShaderResources( 1, 1, &(this->pTextureAlpha.second) );
}

Sniper::Sniper()
{
	this->pColorMapSRV = NULL;
	this->pColorMapRTV = NULL;
	this->pDepthMapSRV = NULL;
	this->pDepthMapDSV = NULL;

	this->pTextureAlpha.first		 = "Texture/Sniper2.png";
	this->pVertexBuffer.first        = "PlyFiles/1x1_2Tri_Quad_2_Sided.ply";
	this->pIndexBuffer.first         = "PlyFiles/1x1_2Tri_Quad_2_Sided.ply";

	this->pCBChangesEveryFrame.first = "CBChangeEveryFrame";
	this->pRastersizerState.first	 = "FillSolid";

	this->pInputLayout.first         = "ShaderFiles/6_Binoculars.fx";
	this->pVertexShader.first        = "ShaderFiles/6_Binoculars.fx";
	this->pPixelShader.first         = "ShaderFiles/6_Binoculars.fx";

	this->ShaderInput.FileName		 = "ShaderFiles/6_Binoculars.fx";
	this->ShaderInput.EntryPoint	 = "VS";	
	this->ShaderInput.Mode			 = "vs_4_0";

	this->ShaderVertex.FileName		 = "ShaderFiles/6_Binoculars.fx";
	this->ShaderVertex.EntryPoint	 = "VS";	
	this->ShaderVertex.Mode			 = "vs_4_0";

	this->ShaderPixel.FileName		 = "ShaderFiles/6_Binoculars.fx";
	this->ShaderPixel.EntryPoint	 = "PS";	
	this->ShaderPixel.Mode			 = "ps_4_0";


	std::wstring error;
	
	bool hr = VertexBuffer::LoadFromPlyFile(L"PlyFiles/1x1_2Tri_Quad_2_Sided.ply", this->vertexBuffer,error); 
	if(!hr)
	{
		throw std::exception("Error loading ply file");
	}
}