#ifndef __APP__
#define __APP__

#include "camera.h"
#include <Windows.h>

class App
{
public:
	virtual void InitWindow( HINSTANCE hInstance, int nCmdShow )                     = 0;
	virtual void InitDevices()                                                       = 0;
	virtual void CleanupDevices()                                                       = 0;
	virtual void Render()                                                               = 0;
	virtual void Run( HINSTANCE hInstance, int nCmdShow )			                    = 0;
	virtual LRESULT CB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) = 0;
	virtual ~App(){}
	static App* getInstance();

	Camera camera;

	// timer stuff
	struct
	{
		double 	  	_absoluteTime;
		double 	  	_frameTime;
		long long	_frameCount;
		double 	  	_frameRate;
	} timer;

	// Windows stuff
	struct
	{
		HINSTANCE	hInst;
		HWND		hWnd;
		UINT		width;
		UINT		height;
	} window;


protected:
	static App* app;
};


#endif //__APP__