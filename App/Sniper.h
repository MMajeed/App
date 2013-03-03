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
	
	virtual float GetOrder();

	virtual void SetupTexture();

	virtual void BuilDepthMap();
	virtual void BuildColorMap();

	ID3D11ShaderResourceView*	pColorMapSRV;
	ID3D11RenderTargetView*		pColorMapRTV;
	
	ID3D11ShaderResourceView*	pDepthMapSRV;
	ID3D11DepthStencilView*		pDepthMapDSV;

	D3D11_VIEWPORT				pViewport;


	
	std::pair<std::string, ID3D11ShaderResourceView*>				pTextureAlpha;
};


#endif //__Sniper__
