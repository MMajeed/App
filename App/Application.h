#ifndef __Application__
#define __Application__

#include "DX11App.h"
#include <vector>
#include "iObjectDrawable.h"

class Application : public DX11App
{
public:
	virtual HRESULT InitDevices();
	virtual void CleanupDevices();
	static App* getInstance();
	virtual LRESULT CB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );	
	virtual ~Application();
	virtual void Run( HINSTANCE hInstance, int nCmdShow );
	
	virtual void Render();
	virtual void ClearScreen();
	virtual void DrawObjects();
	virtual void Present();

	std::vector<iObjectDrawable*> objects;	
protected:	
	std::pair<std::string, ID3D11Buffer*>			pCBNeverChangesID;
	std::pair<std::string, ID3D11Buffer*>			pCBChangesOnResizeID;
	std::pair<std::string, ID3D11SamplerState*>		pSamplerLinear;			
	std::pair<std::string, ID3D11SamplerState*>		pSamplerAnisotropic;
	
	void LoadD3DStuff();
};

#endif