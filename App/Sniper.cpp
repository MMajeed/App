#include "Sniper.h"
#include "Application.h"
#include "MathHelper.h"
#include "Helper.h"
#include "DX11ObjectManager.h"

void Sniper::UpdateDrawing(float delta)
{
	// Remove this object from the list because we don't want to see it
	int counter = 0;
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
	((Application*)App::getInstance())->objects.insert( (((Application*)App::getInstance())->objects.begin() + counter), this);

}
void Sniper::UpdateObject(float delta)
{	
	this->object.Pos = App::getInstance()->camera.Target();

	this->object.Rot.x = App::getInstance()->camera.Pitch() + 1.57f;
	this->object.Rot.y = App::getInstance()->camera.Yaw() ;
	this->object.Rot.z = App::getInstance()->camera.Roll();

	this->object.Scale = XMFLOAT4(0.56f, 1.0f, 0.43f, 0.5f);
}
void Sniper::SetupTexture()
{
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
	
	if(temp2DTexture != 0)
	{
		pImmediateContext->PSSetShaderResources( 0, 1, &(this->temp2DTexture) );
	}
	
	pImmediateContext->PSSetShaderResources( 1, 1, &(this->pTextureAlpha.second) );
}
void Sniper::Init()
{
	BasicObject::Init();

	ID3D11Device* pDevice = ((DX11App*)App::getInstance())->direct3d.pd3dDevice;
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
	UINT width = App::getInstance()->window.width;
	UINT height = App::getInstance()->window.height;

	OffScreen2DTexture.init(pDevice, pImmediateContext, width, height, true, DXGI_FORMAT_R32G32B32A32_FLOAT);

	if(!DX11ObjectManager::getInstance()->Textexture.Exists(this->pTextureAlpha.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadTextureFile(Helper::stringToWstring(this->pTextureAlpha.first), pDevice, &(this->pTextureAlpha.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->Textexture.Add(this->pTextureAlpha.first, this->pTextureAlpha.second);
	}
	if(!DX11ObjectManager::getInstance()->Textexture.Get(this->pTextureAlpha.first, this->pTextureAlpha.second)){ throw std::exception("Texture alpha not found"); }
}
Sniper::Sniper()
{
	this->pTextureAlpha.first		 = "Texture/Binoculars.png";
	temp2DTexture = 0;
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