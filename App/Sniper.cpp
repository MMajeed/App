#include "Sniper.h"
#include "Application.h"

void Sniper::Update(float delta)
{
	int counter = 0;
	// Remove this object from the list because we don't want to see it
	// Object will alway been in the back so check there first by using a reverse iterator
	for(auto objectIter = ((Application*)App::getInstance())->objects.begin();
		objectIter != ((Application*)App::getInstance())->objects.end();
		++objectIter)
	{
		if( (*objectIter) == this)
		{
			((Application*)App::getInstance())->objects.erase(objectIter);
			break;
		}		
		++counter;
	}

	OffScreen2DTexture.begin();

	App::getInstance()->Render();

	OffScreen2DTexture.end();

	temp2DTexture = OffScreen2DTexture.colorMap();

	auto direct3d = ((DX11App*)App::getInstance())->direct3d;
	direct3d.pImmediateContext->OMSetRenderTargets(1, &direct3d.pRenderTargetView, direct3d.pDepthStencilView);
	
	((Application*)App::getInstance())->objects.insert(( ((Application*)App::getInstance())->objects.begin() + counter), this);

}
void Sniper::SetupTexture()
{
	if(temp2DTexture != 0)
	{
		ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
	
		pImmediateContext->PSSetShaderResources( 0, 1, &(this->temp2DTexture) );
		pImmediateContext->PSSetShaderResources( 1, 1, &(this->temp2DTexture) );
	}
}
void Sniper::Init()
{
	BasicObject::Init();

	ID3D11Device* pDevice = ((DX11App*)App::getInstance())->direct3d.pd3dDevice;
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
	UINT width = App::getInstance()->window.width;
	UINT height = App::getInstance()->window.height;

	OffScreen2DTexture.init(pDevice, pImmediateContext, width, height, true, DXGI_FORMAT_R32G32B32A32_FLOAT);
}
Sniper::Sniper()
{
	this->object.Rot.x = 1.57f;

	temp2DTexture = 0;
	this->pVertexBuffer.first        = "PlyFiles/1x1_2Tri_Quad_2_Sided.ply";
	this->pIndexBuffer.first         = "PlyFiles/1x1_2Tri_Quad_2_Sided.ply";

	this->pCBChangesEveryFrame.first = "CBChangeEveryFrame";
	this->pRastersizerState.first	 = "FillSolid";

	this->pInputLayout.first         = "ShaderFiles/2_TwoTexturesOnly_DX11.fx";
	this->pVertexShader.first        = "ShaderFiles/2_TwoTexturesOnly_DX11.fx";
	this->pPixelShader.first         = "ShaderFiles/2_TwoTexturesOnly_DX11.fx";

	this->ShaderInput.FileName		 = "ShaderFiles/2_TwoTexturesOnly_DX11.fx";
	this->ShaderInput.EntryPoint	 = "VS";	
	this->ShaderInput.Mode			 = "vs_4_0";

	this->ShaderVertex.FileName		 = "ShaderFiles/2_TwoTexturesOnly_DX11.fx";
	this->ShaderVertex.EntryPoint	 = "VS";	
	this->ShaderVertex.Mode			 = "vs_4_0";

	this->ShaderPixel.FileName		 = "ShaderFiles/0_Basic.fx";
	this->ShaderPixel.EntryPoint	 = "PS";	
	this->ShaderPixel.Mode			 = "ps_4_0";


	std::wstring error;
	
	bool hr = VertexBuffer::LoadFromPlyFile(L"PlyFiles/1x1_2Tri_Quad_2_Sided.ply", this->vertexBuffer,error); 
	if(!hr)
	{
		throw std::exception("Error loading ply file");
	}	
}