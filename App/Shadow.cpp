#include "Shadow.h"
#include "Application.h"

bool						Shadow::IsInited		= false;
ID3D11ShaderResourceView*	Shadow::pColorMapSRV	= NULL;
ID3D11RenderTargetView*		Shadow::pColorMapRTV	= NULL;


void Shadow::CreateShadow()
{
	if(Shadow::IsInited == false)
	{
		Shadow::Init();
		Shadow::IsInited = true;
	}
	Application* app = Application::getInstance();

	auto d3dStuff = app->direct3d;

	ID3D11RenderTargetView* renderTargets[1] = {Shadow::pColorMapRTV};
	d3dStuff.pImmediateContext->OMSetRenderTargets(1, renderTargets, d3dStuff.pDepthStencilView);

	float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	d3dStuff.pImmediateContext->ClearRenderTargetView(Shadow::pColorMapRTV, black);

	d3dStuff.pImmediateContext->ClearDepthStencilView(d3dStuff.pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	std::map<std::string, ObjectInfo>& objects = app->objects;

	for(auto iter = objects.begin();
		iter != objects.end();
		++iter)
	{
		iter->second.ObjectDrawable->DrawDepth();
	}

	d3dStuff.pImmediateContext->GenerateMips(pColorMapSRV);

	d3dStuff.pImmediateContext->OMSetRenderTargets(1, &(d3dStuff.pRenderTargetView), d3dStuff.pDepthStencilView);	

	if(Shadow::pColorMapSRV != 0)
	{
		d3dStuff.pImmediateContext->PSSetShaderResources( 10, 1, &(Shadow::pColorMapSRV) );
	}
}

void Shadow::Init()
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
	hr = d3dStuff.pd3dDevice->CreateRenderTargetView(colorMap, 0, &(Shadow::pColorMapRTV));
    if(hr)
	{
		throw std::exception("Failed at creating render target view");
	}

	hr = d3dStuff.pd3dDevice->CreateShaderResourceView(colorMap, 0, &(Shadow::pColorMapSRV));
	if(hr)
	{
		throw std::exception("Failed at creating shader resource view");
	}

    // View saves a reference to the texture so we can
    // release our reference.
	colorMap->Release();
}
