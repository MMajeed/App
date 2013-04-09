#ifndef __Application__
#define __Application__

#include "DX11App.h"
#include <vector>
#include "iObjectDrawable.h"
#include "Prespective.h"
#include "Mesh.h"
#include "FBXObject.h"
#include "SkeletalAnimation.h"
#include "LightManager.h"
#include "ObjectInfo.h"
class Application : public DX11App
{
public:
	virtual void InitDevices();
	virtual void CleanupDevices();
	static Application* getInstance();
	virtual LRESULT CB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );	
	virtual ~Application();
	virtual void Run( HINSTANCE hInstance, int nCmdShow );
	
	virtual void Render();
	virtual void SetupDepthTexture();
	virtual void ClearScreen();
	virtual void SetupDraw();
	virtual void SetupSampler();
	virtual void SetupCBNeverChanges();
	virtual void SetupCBChangesOnResize();
	virtual void DrawObjects();
	virtual void Present();

	virtual void SortObject();
	std::map<std::string, ObjectInfo> objects;	

	Prespective Projection;
protected:	
	std::pair<std::string, ID3D11Buffer*>			pCBNeverChangesID;
	std::pair<std::string, ID3D11Buffer*>			pCBChangesOnResizeID;
	std::pair<std::string, ID3D11SamplerState*>		pSamplerLinear;			
	std::pair<std::string, ID3D11SamplerState*>		pSamplerAnisotropic;
	
	void LoadD3DStuff();

	LightManager lightManager;
};

#endif