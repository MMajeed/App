#ifndef __Sniper__
#define __Sniper__

#include "BasicObject.h"

class Sniper : public BasicObject
{
public:	
	Sniper();
	
	virtual void Init();

	virtual void GetNewDynamicTexture();
	virtual void UpdateDrawing(float delta);
	virtual void UpdateObject(float delta);

	virtual void SetupTexture();

	virtual void BuildColorMap();

	ID3D11ShaderResourceView*	pColorMapSRV;
	ID3D11RenderTargetView*		pColorMapRTV;

	virtual iObjectDrawable* clone() const;
	
	std::pair<std::string, ID3D11ShaderResourceView*>				pTextureAlpha;
};


#endif //__Sniper__
