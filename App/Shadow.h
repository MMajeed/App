#ifndef _SHADOW_H_
#define _SHADOW_H_

#include <Windows.h>

struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
struct D3D11_VIEWPORT;
class Shadow
{
public:
	static void CreateShadow();
	static void Init();

	static bool IsInited;

	static ID3D11ShaderResourceView* mDepthMapSRV;
	static ID3D11DepthStencilView* mDepthMapDSV;

	static D3D11_VIEWPORT mViewport;

};

#endif //_SHADOW_H_