#ifndef __SkyBox__
#define __SkyBox__

#include "Object.h"
#include "BasicObject.h"
#include "DX11Helper.h"
#include "VertexBuffer.h"
#include <string>
#include <map>

class SkyBox : public BasicObject
{
public:	
	SkyBox();

	virtual void Update(float delta);
	virtual void InitRastersizerState(ID3D11Device* device);

	static SkyBox* Spawn(std::map<std::string, std::string> info);
};


#endif //__BasicObject__