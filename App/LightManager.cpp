#include "LightManager.h"

LightManager::LightManager(std::size_t number)
{
	this->NumberOfLights = number;
	this->Lights.resize(number);
}
LightManager::~LightManager()
{

}

Light& LightManager::operator[](const int Index)
{
	std::size_t index = Index;

	if(index > this->NumberOfLights)
	{
		index = this->NumberOfLights - 1;
	}

	return this->Lights.at(index);
}

cBuffer::CLightDesc LightManager::GetLightBuffer(std::size_t index)
{
	cBuffer::CLightDesc returnObject;
	Light lightObject = this->operator[](index);

	returnObject.pos = lightObject.pos;
	returnObject.dir = lightObject.dir;
	returnObject.attenuation = lightObject.attenuation;

	returnObject.material.diffuse = lightObject.diffuse;
	returnObject.material.ambient = lightObject.ambient;
	returnObject.material.spec = lightObject.spec;

	returnObject.lightPowerRangeType.x = lightObject.power;
	returnObject.lightPowerRangeType.y = lightObject.range;
	returnObject.lightPowerRangeType.z = static_cast<float>(lightObject.type);
	returnObject.lightPowerRangeType.w = 0.0f;
	return returnObject;
}

XMFLOAT4X4 LightManager::GetViewMatrix(std::size_t index)
{
	Light lightObject = this->operator[](index);

	XMVECTOR Eye = XMVectorSet( lightObject.pos.x, lightObject.pos.y,  lightObject.pos.z, lightObject.pos.w );
	XMVECTOR At = XMVectorSet( lightObject.dir.x, lightObject.dir.y, 1.0f, lightObject.dir.w );
    XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	XMFLOAT4X4  view;
	XMStoreFloat4x4(&view, XMMatrixLookAtLH( Eye, At, Up ));

	return view;
}