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