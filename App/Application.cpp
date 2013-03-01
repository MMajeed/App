#include "Application.h"
#include "PlyFile.h"
#include "SkyBox.h"
#include "DX11ObjectManager.h"
#include "Helper.h"
#include "DX11Helper.h"
#include "cBuffer.h"
#include "ObjectLoader.h"
#include "MathHelper.h"
#include "Sniper.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "SphericalMirror.h"

void Application::Render()
{
	this->ClearScreen();
	this->DrawObjects();
	this->Present();	
}
void Application::ClearScreen()
{
	// Clear the back buffer 
	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // red,green,blue,alpha
	this->direct3d.pImmediateContext->ClearRenderTargetView( this->direct3d.pRenderTargetView, ClearColor );
	this->direct3d.pImmediateContext->ClearDepthStencilView( this->direct3d.pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void Application::DrawObjects()
{
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;

	pImmediateContext->PSSetSamplers( 0, 1, &(this->pSamplerLinear.second) );
	pImmediateContext->PSSetSamplers( 1, 1, &(this->pSamplerAnisotropic.second) );
	
	cBuffer::cbNeverChanges cbCNV ;

	cbCNV.mView = XMMatrixTranspose(XMLoadFloat4x4(&camera.GetViewMatrix()));
	cbCNV.eye = App::getInstance()->camera.Eye();
	cbCNV.target = App::getInstance()->camera.Target();

	float minViewable = 0.01f;
	float maxViewable = 100000.0f;

	cBuffer::cbChangeOnResize cbCOR ;
	cbCOR.mProjection = XMMatrixTranspose(XMMatrixPerspectiveFovLH( XM_PIDIV4, 
											static_cast<float>(this->window.width) / static_cast<float>(this->window.height), 
											minViewable, maxViewable ));

	pImmediateContext->UpdateSubresource( this->pCBNeverChangesID.second, 0, NULL, &cbCNV, 0, 0 );
	pImmediateContext->VSSetConstantBuffers( 0, 1, &this->pCBNeverChangesID.second );
	pImmediateContext->PSSetConstantBuffers( 0, 1, &this->pCBNeverChangesID.second );
	pImmediateContext->UpdateSubresource( this->pCBChangesOnResizeID.second, 0, NULL, &cbCOR, 0, 0 );
	pImmediateContext->VSSetConstantBuffers( 1, 1, &this->pCBChangesOnResizeID.second );
	pImmediateContext->PSSetConstantBuffers( 1, 1, &this->pCBChangesOnResizeID.second );

	
	struct SortFromCamera
	{
		bool operator()(iObjectDrawable* v1, iObjectDrawable* v2)
		{			
			// Make Sniper first
			if( dynamic_cast<Sniper*>(v1) != 0) return true;
			if( dynamic_cast<Sniper*>(v2) != 0) return false;

			// Make skybox first
			if( dynamic_cast<SkyBox*>(v1) != 0) return true;
			if( dynamic_cast<SkyBox*>(v2) != 0) return false;

			// Make SphericalMirror first
			if( dynamic_cast<SphericalMirror*>(v1) != 0) return true;
			if( dynamic_cast<SphericalMirror*>(v2) != 0) return false;

			// Otherwise order them by how far away they are
			float v1Distance = MathHelper::Length(v1->object.Pos, App::getInstance()->camera.Eye());
			float v2Distance = MathHelper::Length(v2->object.Pos, App::getInstance()->camera.Eye());

			return v1Distance > v2Distance;
		}
	};

	std::sort(this->objects.begin(), this->objects.end(), SortFromCamera());

	for(std::size_t i = 0; i < this->objects.size(); ++i)
	{
		this->objects[i]->Draw();
	}
}
void  Application::Present()
{
	// Present the information rendered to the back buffer to the front buffer (the screen)
	this->direct3d.pSwapChain->Present( 0, 0 );
}
void Application::Run( HINSTANCE hInstance, int nCmdShow )
{
	this->InitWindow(hInstance, nCmdShow);
	this->InitDevices();

	SetTimer( this->window.hWnd, FRAMERATE_UPDATE_TIMER, 100, NULL );

	// setup the frame timer
	LARGE_INTEGER timerFrequency = { 0 };
	LARGE_INTEGER timerBase = { 0 };
	LARGE_INTEGER timerLast = { 0 };
	LARGE_INTEGER timerNow = { 0 };
	if( !QueryPerformanceFrequency( &timerFrequency ) ) 
		throw std::exception( "QueryPerformanceFrequency() failed to create a high-performance timer." );
	double tickInterval = static_cast<double>( timerFrequency.QuadPart );

	if( !QueryPerformanceCounter( &timerBase ) )
		throw std::exception( "QueryPerformanceCounter() failed to read the high-performance timer." );
	timerLast = timerBase;
	this->timer._frameCount = 0;

	// Main message loop
	MSG msg = {0};
	while( WM_QUIT != msg.message )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			// update timer
			if( !QueryPerformanceCounter( &timerNow ) )
				throw std::exception( "QueryPerformanceCounter() failed to update the high-performance timer." );
			long long elapsedCount = timerNow.QuadPart - timerBase.QuadPart;
			long long elapsedFrameCount = timerNow.QuadPart - timerLast.QuadPart;
			timer._absoluteTime = elapsedCount / tickInterval;
			timer._frameTime = elapsedFrameCount / tickInterval;
						
			//Sleep( 10 - timer._frameTime); // Used to lock the framerate. Turned off because I want to see how fast the program is going
			
			for(std::size_t i = 0; i < this->objects.size(); ++i)
			{
				objects[i]->UpdateDrawing(static_cast<float>(timer._frameTime));
			}

			for(std::size_t i = 0; i < this->objects.size(); ++i)
			{
				objects[i]->UpdateObject(static_cast<float>(timer._frameTime));
			}

			// render
			this->Render();
			this->Present();
			// update fps
			timerLast = timerNow;
			++(timer._frameCount);
		}
	}

	KillTimer( this->window.hWnd, FRAMERATE_UPDATE_TIMER );
}
HRESULT Application::InitDevices()
{
	DX11App::InitDevices();
		
	this->pCBNeverChangesID.first		= "CBNeverChange";
	this->pCBChangesOnResizeID.first	= "CBChangesOnResize";
	this->pSamplerLinear.first			= "SamplerLinear";
	this->pSamplerAnisotropic.first		= "SamplerAnisotropic";

	ID3D11Device* device = ((DX11App*)App::getInstance())->direct3d.pd3dDevice;	
	std::wstring error;

	if(!DX11ObjectManager::getInstance()->CBuffer.Exists(this->pCBNeverChangesID.first))
	{
		if(!cBuffer::LoadBuffer<cBuffer::cbNeverChanges>(device, &(this->pCBNeverChangesID.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->CBuffer.Add(this->pCBNeverChangesID.first, this->pCBNeverChangesID.second);
	}

	if(!DX11ObjectManager::getInstance()->CBuffer.Exists(this->pCBChangesOnResizeID.first))
	{
		if(!cBuffer::LoadBuffer<cBuffer::cbChangeOnResize>(device, &(this->pCBChangesOnResizeID.second), error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->CBuffer.Add(this->pCBChangesOnResizeID.first, this->pCBChangesOnResizeID.second);
	}

	if(!DX11ObjectManager::getInstance()->Sampler.Exists(this->pSamplerLinear.first))
	{
		if(!DX11Helper::LoadSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_COMPARISON_NEVER,
										0.0f,
										D3D11_FLOAT32_MAX, device,
										&(this->pSamplerLinear.second),
										error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->Sampler.Add(this->pSamplerLinear.first, this->pSamplerLinear.second);
	}

	if(!DX11ObjectManager::getInstance()->Sampler.Exists(this->pSamplerAnisotropic.first))
	{
		if(!DX11Helper::LoadSamplerState(D3D11_FILTER_ANISOTROPIC,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_COMPARISON_NEVER,
										0.0f,
										D3D11_FLOAT32_MAX,
										device,
										&(this->pSamplerAnisotropic.second),
										error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->Sampler.Add(this->pSamplerAnisotropic.first, this->pSamplerAnisotropic.second);
	}

	this->LoadD3DStuff();
	
	ObjectLoader::getInstance()->LoadXMLFile("Commands.xml");

	objects = ObjectLoader::getInstance()->SpawnAll();
 
	SphericalMirror* dsb = new SphericalMirror();
	dsb->Init();
	objects.push_back(dsb);

	return true;
}
LRESULT Application::CB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
    {
		case WM_KEYDOWN: 
            switch (wParam) 
            { 
                case VK_LEFT: 
					camera.Yaw(-0.1f);	// Move "left"
                    return 0;
				case VK_RIGHT:
					camera.Yaw(+0.1f);	    // Move "right"
					return 0;
				case VK_UP:
					camera.MoveForward(0.5f);	    // Move "up"
					return 0;
				case VK_DOWN:
					camera.MoveForward(-0.5f);	// Move "down"
					return 0;
				case VK_NEXT:	            // PgUp
					camera.Pitch(0.1f);	    // Move "towards"
					return 0;
				case VK_PRIOR:	            // PgDown
					camera.Pitch(-0.1f);	// Move "away"
					return 0;
				case 'A': case 'a':	            // PgDown
					{
						((BasicObject*)this->objects.front())->object.Pos.x += 1.0f;;
					}
					return 0;
				case 'Z': case 'z':
					{
						bool found = false;
						for(auto objectIter = this->objects.begin();
							objectIter != this->objects.end();
							++objectIter)
						{
							if( dynamic_cast<Sniper*>(*objectIter) != 0)
							{
								found = true;
								this->objects.erase(objectIter);
								break;
							}
						}

						if(found == false)
						{
							Sniper* sniper = new Sniper;
							sniper->Init();
							this->objects.push_back(sniper);
						}
					}
					return 0;
			}
		case WM_TIMER:
		{
			switch( wParam )
			{
				case FRAMERATE_UPDATE_TIMER:
				/* Update the frame rate and its string representation. */
				{
					this->timer._frameRate = this->timer._frameCount / this->timer._absoluteTime;
					std::wostringstream fr;
					fr << std::setprecision(2) << std::fixed << this->timer._frameRate;

					std::wostringstream caption;
					caption << "Framerate: " << fr.str() << " ";
					SetWindowTextW( hWnd, caption.str().c_str() );
					break;
				}
			}
			return 0;
		}
	}
	return DX11App::CB_WndProc(hWnd, message, wParam, lParam);
}	
void Application::CleanupDevices()
{
	DX11App::InitDevices();

	while(this->objects.size() >= 0)
	{
		(*this->objects.begin())->Clean();
		delete (*this->objects.begin());
		this->objects.erase(this->objects.begin());		
	}
}
void Application::LoadD3DStuff()
{
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->pCBNeverChangesID.first, this->pCBNeverChangesID.second)){ throw std::exception("CB never Changes not found"); }
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->pCBChangesOnResizeID.first, this->pCBChangesOnResizeID.second)){ throw std::exception("CB Changes on Resize not found"); }
	if(!DX11ObjectManager::getInstance()->Sampler.Get(this->pSamplerLinear.first, this->pSamplerLinear.second)){ throw std::exception("Sampler Linear not found"); }
	if(!DX11ObjectManager::getInstance()->Sampler.Get(this->pSamplerAnisotropic.first, this->pSamplerAnisotropic.second)){ throw std::exception("Sampler Anisotropic not found"); }
}
App* Application::getInstance()
{
	if(App::app == 0)
	{
		app = new Application;
	}

    return app;
}
Application::~Application()
{
}