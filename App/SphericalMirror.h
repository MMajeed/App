#ifndef __DynmaicSkyBox__
#define __DynmaicSkyBox__

#include "BasicObject.h"

class SphericalMirror : public BasicObject
{
public:	
	SphericalMirror();
	
	virtual void Init();

	virtual void GetNewDynamicTexture();
	virtual void UpdateDrawing(float delta);
	virtual void UpdateObject(float delta);

	virtual void SetupTexture();

	ID3D11DepthStencilView*		pDynamicCubeMapDSV;
	ID3D11RenderTargetView*		pDynamicCubeMapRTV[6];
	ID3D11ShaderResourceView*	pDynamicCubeMapSRV;
	D3D11_VIEWPORT				pCubeMapViewport;

	static const unsigned int CubeMapSize = 256;
};


#endif //__DynmaicSkyBox__
