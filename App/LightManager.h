#ifndef __LightManager_HPP__
#define __LightManager_HPP__

#include "Light.h"
#include <vector>
#include "cBuffer.h"
class LightManager
{
public:
	LightManager(std::size_t number = 10);
	virtual ~LightManager();

	Light& operator[](const int Index);
	cBuffer::CLightDesc GetLightBuffer(std::size_t index);
protected:
	std::vector<Light> Lights;
	std::size_t NumberOfLights;
};

#endif // __LightManager_HPP__