#ifndef __Application__
#define __Application__

#include "DX11App.h"
#include <vector>
#include "iObjectDrawable.h"
#include "Prespective.h"
#include "Mesh.h"
#include "DisplayMeshGPU.h"
#include "SkeletalAnimation.h"

class Application : public DX11App
{
public:
	virtual void InitDevices();
	virtual void CleanupDevices();
	static App* getInstance();
	virtual LRESULT CB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );	
	virtual ~Application();
	virtual void Run( HINSTANCE hInstance, int nCmdShow );
	
	virtual void Render();
	virtual void ClearScreen();
	virtual void DrawObjects();
	virtual void Present();

	virtual void SortObject();
	std::vector<iObjectDrawable*> objects;	

	Prespective Projection;
protected:	
	std::pair<std::string, ID3D11Buffer*>			pCBNeverChangesID;
	std::pair<std::string, ID3D11Buffer*>			pCBChangesOnResizeID;
	std::pair<std::string, ID3D11SamplerState*>		pSamplerLinear;			
	std::pair<std::string, ID3D11SamplerState*>		pSamplerAnisotropic;
	
	Mesh*                   g_pMyMesh;
	DisplayMeshGPU*         g_pMyDisplayMeshGPU;
	SkeletalAnimation*      g_pIdleAnim;
	SkeletalAnimation*      g_pWalkAnim;
	SkeletalAnimation*      g_pRunAnim;

	void LoadD3DStuff();
};

#endif