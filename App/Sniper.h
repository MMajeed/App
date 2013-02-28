#ifndef __Sniper__
#define __Sniper__

#include "BasicObject.h"
#include "CDrawableTex2D_DX11.h"

class Sniper : public BasicObject
{
public:	
	Sniper();
	
	virtual void Update(float delta);

	virtual void Init();


	virtual void SetupTexture();

	CDrawableTex2D_DX11		OffScreen2DTexture;

	ID3D11ShaderResourceView *temp2DTexture;
};


#endif //__Sniper__
