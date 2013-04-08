#ifndef _SHADOW_H_
#define _SHADOW_H_

struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;

class Shadow
{
public:
	static void CreateShadow();
	static void Init();

	static bool IsInited;
	static ID3D11ShaderResourceView*		pColorMapSRV;
	static ID3D11RenderTargetView*	pColorMapRTV;
};

#endif //_SHADOW_H_