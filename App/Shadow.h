#ifndef _SHADOW_H_
#define _SHADOW_H_

struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;

class Shadow
{
public:
	static void CreateShadow();
	static void Init();
	static void BuildDepthMap();

	static bool IsInited;
	static ID3D11ShaderResourceView*	pDepthMapSRV;
	static ID3D11DepthStencilView*		pDepthMapDSV;
};

#endif //_SHADOW_H_