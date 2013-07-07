#include "Sniper.h"
#include "Application.h"
#include "Helper.h"
#include "DX11ObjectManager.h"

void Sniper::GetNewDynamicTexture()
{
	auto d3dStuff = DX11App::getInstance()->direct3d;

	ID3D11RenderTargetView* renderTargets[1] = {this->pColorMapRTV};
	d3dStuff.pImmediateContext->OMSetRenderTargets(1, renderTargets, d3dStuff.pDepthStencilView);

	float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	d3dStuff.pImmediateContext->ClearRenderTargetView(this->pColorMapRTV, black);

	d3dStuff.pImmediateContext->ClearDepthStencilView(d3dStuff.pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	Application::getInstance()->DrawObjects();

	d3dStuff.pImmediateContext->GenerateMips(pColorMapSRV);

	d3dStuff.pImmediateContext->OMSetRenderTargets(1, &(d3dStuff.pRenderTargetView), d3dStuff.pDepthStencilView);	
}
void Sniper::UpdateDrawing(float delta)
{
	UNREFERENCED_PARAMETER(delta);

	// Remove this object from the list that we don't want to see	
	std::vector<std::string> removed;
	std::map<std::string, ObjectInfo>& applciationList = Application::getInstance()->objects;
	for(auto iter = applciationList.begin();
		iter != applciationList.end();
		++iter)
	{
		if( dynamic_cast<Sniper*>(iter->second.ObjectDrawable) != 0)
		{
			removed.push_back(iter->first);
			iter->second.DrawNext = false;
		}
	}

	// Get the new texture
	this->GetNewDynamicTexture();

	// Put back the ones we removed
	for(auto removedIter = removed.begin();
		removedIter != removed.end();
		++removedIter)
	{
		applciationList[*removedIter].DrawNext = true;
	}
}
void Sniper::UpdateObject(float delta)
{
	UNREFERENCED_PARAMETER(delta);

	this->object.Pos = App::getInstance()->camera.Target();

	this->object.Rot.x = App::getInstance()->camera.Pitch() + 1.57f;
	this->object.Rot.y = App::getInstance()->camera.Yaw() ;
	this->object.Rot.z = App::getInstance()->camera.Roll() ;

	this->object.Scale = XMFLOAT4(0.56f, 1.0f, 0.43f, 0.5f);
}

void Sniper::SetupDrawTexture()
{
	ID3D11DeviceContext* pImmediateContext = DX11App::getInstance()->direct3d.pImmediateContext;

	if(this->pColorMapSRV != 0)
	{
		pImmediateContext->PSSetShaderResources( 0, 1, &(this->pColorMapSRV) );
		pImmediateContext->PSSetShaderResources( 10, 1, &(this->pColorMapSRV) );
	}
	
	pImmediateContext->PSSetShaderResources( 1, 1, &(this->pTextureAlpha.second) );
}

void Sniper::BuildColorMap()
{
	auto d3dStuff = DX11App::getInstance()->direct3d;

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
	
	this->BuildColorMap();

	auto d3dStuff = DX11App::getInstance()->direct3d;

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
Sniper::Sniper()
{
	this->pColorMapSRV = NULL;
	this->pColorMapRTV = NULL;

	this->pTextureAlpha.first		 = "../Resources/Texture/Sniper2.png";
	this->pVertexBuffer.first        = "../Resources/PlyFiles/1x1_2Tri_Quad_2_Sided.ply";
	this->pIndexBuffer.first         = "../Resources/PlyFiles/1x1_2Tri_Quad_2_Sided.ply";

	this->pCBChangesEveryFrame.first = "CBChangeEveryFrame";
	this->pRastersizerState.first	 = "FillSolid";

	this->pInputLayout.first         = "../Resources/ShaderFiles/VS_0_Regular.fx";
	this->pVertexShader.first        = "../Resources/ShaderFiles/VS_0_Regular.fx";
	this->pPixelShader.first         = "../Resources/ShaderFiles/PS_6_Binoculars.fx";

	this->Shader.ShaderVertex.FileName		 = "../Resources/ShaderFiles/VS_0_Regular.fx";
	this->Shader.ShaderVertex.EntryPoint	 = "VS";	
	this->Shader.ShaderVertex.Mode			 = "vs_4_0";

	this->Shader.ShaderPixel.FileName		 = "../Resources/ShaderFiles/PS_6_Binoculars.fx";
	this->Shader.ShaderPixel.EntryPoint		 = "PS";	
	this->Shader.ShaderPixel.Mode			 = "ps_4_0";


	std::wstring error;
	
	bool hr = PlyBuffer::LoadFromPlyFile(L"../Resources/PlyFiles/1x1_2Tri_Quad_2_Sided.ply", this->PlyBuffer,error); 
	if(!hr)
	{
		throw std::exception("Error loading ply file");
	}
}

iObjectDrawable* Sniper::clone() const
{
	return new Sniper(*this);
}
