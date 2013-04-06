#include "Transparent.h"
#include "DX11ObjectManager.h"
#include "DX11App.h"
#include "DX11Helper.h"
#include "Helper.h"

void Transparent::DrawObject()
{
	ID3D11DeviceContext* pImmediateContext = DX11App::getInstance()->direct3d.pImmediateContext;
		
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	pImmediateContext->OMSetBlendState(this->pTransparent.second, blendFactor, 0xffffffff);

	PlyFile::DrawObject();

	pImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
}
void Transparent::Init()
{
	PlyFile::Init();

	ID3D11Device* device = DX11App::getInstance()->direct3d.pd3dDevice;

	this->LoadTransparency(device);

	if(!DX11ObjectManager::getInstance()->BelnderState.Get(this->pTransparent.first, this->pTransparent.second)){ throw std::exception("Transparent blender not found"); }
}
void Transparent::LoadTransparency(ID3D11Device* device)
{
	if(!DX11ObjectManager::getInstance()->BelnderState.Exists(this->pTransparent.first))
	{
		std::wstring error;
		if(!DX11Helper::LoadTransparent(device, &(this->pTransparent.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->BelnderState.Add(this->pTransparent.first , this->pTransparent.second);
	}
}

float Transparent::GetOrder()
{
	return MathHelper::Length(this->object.Pos, App::getInstance()->camera.Eye());
}

Transparent::Transparent()
{
	this->pTransparent.first = "Transparent";
}
Transparent::Transparent(PlyFile base)
	: PlyFile(base)
{

}
Transparent* Transparent::Spawn(std::map<std::string, std::string> info)
{
	PlyFile* plyObject = PlyFile::Spawn(info);
	Transparent* newTransparent = new Transparent(*plyObject);
	delete plyObject;
	return newTransparent;
}

iObjectDrawable* Transparent::clone() const
{
	return new Transparent(*this);
}