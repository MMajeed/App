#ifndef __SkyBox__
#define __SkyBox__

#include "Object.h"
#include "BasicObject.h"
#include "DX11Helper.h"
#include "VertexBuffer.h"
#include <string>

class SkyBox : public BasicObject
{
public:	
	SkyBox(std::string cubeMap);

	virtual void Update(float delta);
	virtual void InitRastersizerState(ID3D11Device* device);
};


#endif //__BasicObject__