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
protected:	
	virtual void Render();
	std::vector<iObjectDrawable*> objects;
	
	std::string		CBNeverChangesID;
	std::string		CBChangesOnResizeID;
	std::string		SamplerLinear;			
	std::string		SamplerAnisotropic;
	
};

#endif