#include "SphericalMirror.h"
#include "Helper.h"
#include "Application.h"
#include "camera.h"
#include "Sniper.h"
#include "Prespective.h"

void SphericalMirror::GetNewDynamicTexture()
{
	auto d3dStuff = ((DX11App*)App::getInstance())->direct3d;

	// This call needs to be called six times, for each direction of the camera.
	// You set the back buffer to be the particular direction in the cube map
	// (note that each direction is pre-defined: 
	//	0 ==> +X	//	1 ==> -X
	//	2 ==> +Y	//	3 ==> -Y
	//	4 ==> +Z	//	5 ==> -Y
	enum enumCamDirection
	{
		X_POS = 0,
		X_NEG,	// = 1
		Y_POS,	// = 2
		Y_NEG,	// = 3
		Z_POS,	// = 4
		Z_NEG	// = 5 
	};
	
	Camera oldCamera = App::getInstance()->camera;
	
	Prespective oldProjection = ((Application*)App::getInstance())->Projection;

	Prespective newProjection;
	newProjection.SetFovAngle(0.5f*XM_PI);
	newProjection.SetHeight(static_cast<float>(this->CubeMapSize));
	newProjection.SetWidth(static_cast<float>(this->CubeMapSize));
	((Application*)App::getInstance())->Projection = newProjection;	

	ID3D11RenderTargetView* renderTargets[1];
	// Generate the cube map.
	d3dStuff.pImmediateContext->RSSetViewports(1, &pCubeMapViewport);
	for(int curCamDir = 0; curCamDir < 6; curCamDir++)
	{
		// Clear cube map face and depth buffer.
		float ColourBlack[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha
		d3dStuff.pImmediateContext->ClearRenderTargetView(pDynamicCubeMapRTV[curCamDir], ColourBlack);
		d3dStuff.pImmediateContext->ClearDepthStencilView(pDynamicCubeMapDSV, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Bind cube map face as render target.
		renderTargets[0] = pDynamicCubeMapRTV[curCamDir];
		d3dStuff.pImmediateContext->OMSetRenderTargets(1, renderTargets, pDynamicCubeMapDSV);

		Camera newCamera;
		newCamera.SetPosition(this->object.Pos.x, this->object.Pos.y, this->object.Pos.z);

		// Draw the scene with the exception of the center sphere to this cube map face.
		switch (curCamDir)
		{
		case X_POS:		// 0
			newCamera.SetUp(0.0f, 1.0f, 0.0f);
			newCamera.SetLook(0.01f, 0.0f, 0.0f); // The look is relative to the camera position
			break;
		case X_NEG:		// 1			
			newCamera.SetUp(0.0f, 1.0f, 0.0f);
			newCamera.SetLook(-0.01f, 0.0f, 0.0f);
			break;
		case Y_POS:		// 2	
			newCamera.SetUp(0.0f, 0.0f, 1.0f);
			newCamera.SetLook(0.0f, 0.01f, 0.0f);
			newCamera.Yaw(3.14f);
			break;
		case Y_NEG:		// 3					
			newCamera.SetUp(0.0f, 0.0f, 1.0f);
			newCamera.SetLook(0.0f, -0.01f, 0.0f);
			break;
		case Z_POS:		// 4				
			newCamera.SetUp(0.0f, 1.0f, 0.0f);			
			newCamera.SetLook(0.0f, 0.0f, 0.01f);
			break;
		case Z_NEG:		// 5
			newCamera.SetUp(0.0f, 1.0f, 0.0f);						
			newCamera.SetLook(0.0f, 0.0f, -0.01f);
			break;
		}

		App::getInstance()->camera = newCamera;
		((Application*)App::getInstance())->SetupDraw();
		((Application*)App::getInstance())->SortObject();
		((Application*)App::getInstance())->DrawObjects();
	}

	// Generate the mip maps for the cube...
    // Have hardware generate lower mipmap levels of cube map.
	d3dStuff.pImmediateContext->GenerateMips(this->pDynamicCubeMapSRV);

	// Restore old viewport and render targets.
	d3dStuff.pImmediateContext->RSSetViewports(1, &(d3dStuff.vp));
	renderTargets[0] = d3dStuff.pRenderTargetView;
	d3dStuff.pImmediateContext->OMSetRenderTargets(1, renderTargets, d3dStuff.pDepthStencilView);

	((Application*)App::getInstance())->Projection = oldProjection;	
	App::getInstance()->camera = oldCamera;

}
void SphericalMirror::UpdateDrawing(float delta)
{
	this->Timer += delta;

	if(this->Timer < this->UpdateEvery)
	{
		return; // Skip over it if it has recently been updated
	}
	
	// If it is time to updated it than reset the timer
	this->Timer = 0.0f; // Reset the timer
	

	// Remove this object from the list that we don't want to see
	std::vector<std::string> removed;
	std::map<std::string, ObjectInfo>& applciationList = ((Application*)App::getInstance())->objects;
	for(auto iter = applciationList.begin();
		iter != applciationList.end();
		++iter)
	{
		if( dynamic_cast<SphericalMirror*>(iter->second.ObjectDrawable) != 0
			|| dynamic_cast<Sniper*>(iter->second.ObjectDrawable) != 0)
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

void SphericalMirror::SetupTexture()
{
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;
	
	if(this->pDynamicCubeMapSRV != 0)
	{
		pImmediateContext->PSSetShaderResources( 0, 1, &pDynamicCubeMapSRV );
	}
}

void SphericalMirror::Init()
{
	BasicObject::Init();

	ID3D11Device* pDevice = ((DX11App*)App::getInstance())->direct3d.pd3dDevice;

	// Cubemap is a special texture array with 6 elements.
	D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width = this->CubeMapSize;
    texDesc.Height = this->CubeMapSize;
    texDesc.MipLevels = 0;
    texDesc.ArraySize = 6;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* cubeTex = 0;
	HRESULT hr = pDevice->CreateTexture2D(&texDesc, 0, &cubeTex);
	if(hr)
	{
		throw std::exception("Failed at creating 6 sides dynamic texture");
	}
	
	// Create a render target view to each cube map face 
	// (i.e., each element in the texture array).
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = texDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    rtvDesc.Texture2DArray.ArraySize = 1;
    rtvDesc.Texture2DArray.MipSlice = 0;

    for(int i = 0; i < 6; ++i)
    {
        rtvDesc.Texture2DArray.FirstArraySlice = i;
        hr = pDevice->CreateRenderTargetView(cubeTex, &rtvDesc, &this->pDynamicCubeMapRTV[i]);

		if(hr)
		{
			throw std::exception("Failed at creating render targer view for dynamic cube map");
		}
    }

    // Create a shader resource view to the cube map.
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(cubeTex, &srvDesc, &this->pDynamicCubeMapSRV);
	if(hr)
	{
		throw std::exception("Failed at creating shader resource view for the dynamic cube map");
	}

	if ( cubeTex )	
	{
		cubeTex->Release();		
		cubeTex = 0;
	}

	// We need a depth texture for rendering the scene into the cubemap
	// that has the same resolution as the cubemap faces.  
	D3D11_TEXTURE2D_DESC depthTexDesc;
    depthTexDesc.Width = this->CubeMapSize;
    depthTexDesc.Height = this->CubeMapSize;
    depthTexDesc.MipLevels = 1;
    depthTexDesc.ArraySize = 1;
    depthTexDesc.SampleDesc.Count = 1;
    depthTexDesc.SampleDesc.Quality = 0;
    depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
    depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthTexDesc.CPUAccessFlags = 0;
    depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTex = 0;
	hr = pDevice->CreateTexture2D(&depthTexDesc, 0, &depthTex);
	if(hr)
	{
		throw std::exception("Failed at creating the texture 2d for the dynamic cube map");
	}

    // Create the depth stencil view for the entire cube
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags  = 0;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
	hr = pDevice->CreateDepthStencilView(depthTex, &dsvDesc, &this->pDynamicCubeMapDSV);
	if(hr)
	{
		throw std::exception("Failed at creating depth stencil view for the dynamic cube map");
	}

	if ( depthTex )
	{
		depthTex->Release();		
		depthTex = 0;
	}

	this->pCubeMapViewport.TopLeftX = 0.0f;
    this->pCubeMapViewport.TopLeftY = 0.0f;
    this->pCubeMapViewport.Width    = static_cast<float>(this->CubeMapSize);
    this->pCubeMapViewport.Height   = static_cast<float>(this->CubeMapSize);
    this->pCubeMapViewport.MinDepth = 0.0f;
    this->pCubeMapViewport.MaxDepth = 1.0f;
}

SphericalMirror::SphericalMirror()
{
	this->UpdateEvery = 0.012f;
	this->Timer = this->UpdateEvery; // you want the timer to start with the same as the update so that it runs on the first render

	this->pVertexBuffer.first        = "../Resources/PlyFiles/Sphere_Smooth_3.ply";
	this->pIndexBuffer.first         = "../Resources/PlyFiles/Sphere_Smooth_3.ply";

	this->pCBChangesEveryFrame.first = "CBChangeEveryFrame";
	this->pRastersizerState.first	 = "FillSolid";

	this->pInputLayout.first         = "../Resources/ShaderFiles/4_CubeMapping.fx";
	this->pVertexShader.first        = "../Resources/ShaderFiles/4_CubeMapping.fx";
	this->pPixelShader.first         = "../Resources/ShaderFiles/4_CubeMapping.fx";

	this->Shader.ShaderInput.FileName		 = "../Resources/ShaderFiles/4_CubeMapping.fx";
	this->Shader.ShaderInput.EntryPoint	 = "VS";	
	this->Shader.ShaderInput.Mode			 = "vs_4_0";

	this->Shader.ShaderVertex.FileName		 = "../Resources/ShaderFiles/4_CubeMapping.fx";
	this->Shader.ShaderVertex.EntryPoint	 = "VS";	
	this->Shader.ShaderVertex.Mode			 = "vs_4_0";

	this->Shader.ShaderPixel.FileName		 = "../Resources/ShaderFiles/4_CubeMapping.fx";
	this->Shader.ShaderPixel.EntryPoint	 = "PS";	
	this->Shader.ShaderPixel.Mode			 = "ps_4_0";

	this->pRastersizerState.first	 = "InsideOut4";

	std::wstring error;
	
	bool hr = PlyBuffer::LoadFromPlyFile(L"../Resources/PlyFiles/Sphere_Smooth_3.ply", this->PlyBuffer,error); 
	if(!hr)
	{
		throw std::exception("Error loading ply file");
	}	

	pDynamicCubeMapSRV = NULL;
	pDynamicCubeMapDSV = NULL;
}

SphericalMirror* SphericalMirror::Spawn(std::map<std::string, std::string> info)
{
	SphericalMirror* newSpericalMirror = new SphericalMirror;
	
	std::wstring error;

	// Scale XYZ
	auto iter = info.find("XYZScaleX");
	if(iter != info.end()) { newSpericalMirror->object.Scale.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZScaleY");
	if(iter != info.end()) { newSpericalMirror->object.Scale.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZScaleZ");
	if(iter != info.end()) { newSpericalMirror->object.Scale.z = Helper::StringToFloat(iter->second); } 
	
	// Location
	iter = info.find("XYZLocationX");
	if(iter != info.end()) { newSpericalMirror->object.Pos.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZLocationY");
	if(iter != info.end()) { newSpericalMirror->object.Pos.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZLocationZ");
	if(iter != info.end()) { newSpericalMirror->object.Pos.z = Helper::StringToFloat(iter->second); } 

	// XYZRotation
	iter = info.find("XYZRotationX");
	if(iter != info.end()) { newSpericalMirror->object.Rot.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZRotationY");
	if(iter != info.end()) { newSpericalMirror->object.Rot.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZRotationZ");
	if(iter != info.end()) { newSpericalMirror->object.Rot.z = Helper::StringToFloat(iter->second); } 

	// XYZOrbit
	iter = info.find("XYZOrbitX");
	if(iter != info.end()) { newSpericalMirror->object.Orbit.x = Helper::StringToFloat(iter->second); } 	
	iter = info.find("XYZOrbitY");
	if(iter != info.end()) { newSpericalMirror->object.Orbit.y = Helper::StringToFloat(iter->second); } 
	iter = info.find("XYZOrbitZ");
	if(iter != info.end()) { newSpericalMirror->object.Orbit.z = Helper::StringToFloat(iter->second); } 

	iter = info.find("UpdateEvery");
	if(iter != info.end()) 
	{
		newSpericalMirror->UpdateEvery = Helper::StringToFloat(iter->second);  
		newSpericalMirror->Timer = Helper::StringToFloat(iter->second);  
	} 

	return newSpericalMirror;
}
