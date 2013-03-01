#ifndef __DX11APP__
#define __DX11APP__

#include "App.h"
#include <d3d11.h>
#include <xnamath.h>

class DX11App : public App
{
public:
	virtual void InitWindow( HINSTANCE hInstance, int nCmdShow );
	virtual void InitDevices();
	virtual void CleanupDevices();
	virtual void Run( HINSTANCE hInstance, int nCmdShow );
	static App* getInstance();
	virtual ~DX11App();
	virtual LRESULT CB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	void Render();

	// DirectX stuff
	struct
	{
		D3D_DRIVER_TYPE				driverType;
		D3D_FEATURE_LEVEL			featureLevel;
		ID3D11Device*				pd3dDevice;
		ID3D11DeviceContext*		pImmediateContext;
		IDXGISwapChain*				pSwapChain;
		ID3D11RenderTargetView*		pRenderTargetView;
		ID3D11Texture2D*			pDepthStencilBuffer;
		ID3D11DepthStencilState*	pDepthStencilState;
		ID3D11DepthStencilView*		pDepthStencilView;
		D3D11_VIEWPORT vp;
	} direct3d;
protected:
	DX11App();
    DX11App(DX11App const&);              
    void operator=(DX11App const&); 
	enum { FRAMERATE_UPDATE_TIMER = 1 };
private:
	static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
};

#endif //__APP__