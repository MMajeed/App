#ifndef __SkyBox__
#define __SkyBox__

#include "BasicObject.h"
#include <string>
#include <map>

class SkyBox : public BasicObject
{
public:	
	SkyBox();

	virtual void UpdateObject(float delta);
	virtual void InitRastersizerState(ID3D11Device* device);

	static SkyBox* Spawn(std::map<std::string, std::string> info);
};


#endif //__BasicObject__