#ifndef __Sniper__
#define __Sniper__

#include "BasicObject.h"
#include "CDrawableTex2D_DX11.h"

class Sniper : public BasicObject
{
public:	
	Sniper();
	
	void UpdateDrawing(float delta);
	void UpdateObject(float delta);

	virtual void Init();


	virtual void SetupTexture();

	CDrawableTex2D_DX11		OffScreen2DTexture;

	ID3D11ShaderResourceView* temp2DTexture;
	std::pair<std::string, ID3D11ShaderResourceView*>				pTextureAlpha;
};


#endif //__Sniper__
