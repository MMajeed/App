#ifndef __DynmaicSkyBox__
#define __DynmaicSkyBox__

#include "BasicObject.h"
#include <map>

class SphericalMirror : public BasicObject
{
public:	
	SphericalMirror();
	
	virtual void Init();

	virtual void GetNewDynamicTexture();
	virtual void UpdateDrawing(float delta);

	virtual void SetupDrawTexture();

	ID3D11DepthStencilView*		pDynamicCubeMapDSV;
	ID3D11RenderTargetView*		pDynamicCubeMapRTV[6];
	ID3D11ShaderResourceView*	pDynamicCubeMapSRV;
	D3D11_VIEWPORT				pCubeMapViewport;

	static const unsigned int CubeMapSize = 1024;
	
	float UpdateEvery;
	float Timer;

	virtual iObjectDrawable* clone() const;

	static SphericalMirror* Spawn(std::map<std::string, std::string> info);
};


#endif //__DynmaicSkyBox__
