#include "DirectXObject.h"
#include "Application.h"
#include "DX11ObjectManager.h"

void DirectXObject::InitDepth()
{
	ID3D11Device* device = DX11App::getInstance()->direct3d.pd3dDevice;
	DirectXObject::InitDepthInputLayout(device);
	DirectXObject::InitDepthVertexShader(device);
	DirectXObject::InitDepthwPixelShader(device);
	DirectXObject::InitDepthRastersizerState(device);
	DirectXObject::InitDepthCBChangesEveryFrameBuffer(device);
}

void DirectXObject::DrawDepth()
{

}

void DirectXObject::InitDepthInputLayout(ID3D11Device* device)
{

}
void DirectXObject::InitDepthVertexShader(ID3D11Device* device)
{
}
void DirectXObject::InitDepthwPixelShader(ID3D11Device* device)
{

}
void DirectXObject::InitDepthRastersizerState(ID3D11Device* device)
{

}
void DirectXObject::InitDepthCBChangesEveryFrameBuffer(ID3D11Device* device)
{

}

DirectXObject::DirectXObject()
{
	BoolDrawDepth = true;
}