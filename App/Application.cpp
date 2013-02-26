#include "Application.h"
#include "PlyFile.h"
#include "SkyBox.h"
#include "DX11ObjectManager.h"
#include "Helper.h"
#include "DX11Helper.h"
#include "cBuffer.h"
#include <sstream>
#include <iomanip>

void Application::Render()
{
	// Clear the back buffer 
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // red,green,blue,alpha
	this->direct3d.pImmediateContext->ClearRenderTargetView( this->direct3d.pRenderTargetView, ClearColor );
	this->direct3d.pImmediateContext->ClearDepthStencilView( this->direct3d.pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	ID3D11DeviceContext* pImmediateContext = ((DX11App*)App::getInstance())->direct3d.pImmediateContext;

	ID3D11SamplerState* samplerLinear;
	if(!DX11ObjectManager::getInstance()->Sampler.Get(this->SamplerLinear, samplerLinear)){ throw std::exception("Sampler Linear not found"); }
	ID3D11SamplerState* samplerAnisotropic;
	if(!DX11ObjectManager::getInstance()->Sampler.Get(this->SamplerAnisotropic, samplerAnisotropic)){ throw std::exception("Sampler Anisotropic not found"); }

	pImmediateContext->PSSetSamplers( 0, 1, &samplerLinear );
	pImmediateContext->PSSetSamplers( 1, 1, &samplerAnisotropic );

	ID3D11Buffer* pCBNeverChanges;
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->CBNeverChangesID , pCBNeverChanges)){ throw std::exception("const buffer not found"); }	
	ID3D11Buffer* pCBChangesOnResize;
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->CBChangesOnResizeID , pCBChangesOnResize)){ throw std::exception("const buffer not found"); }
	
	cBuffer::cbNeverChanges cbCNV ;

	cbCNV.mView = XMMatrixTranspose(XMLoadFloat4x4(&camera.Update()));
	cbCNV.eye = App::getInstance()->camera.eye;
	cbCNV.target = App::getInstance()->camera.target;

	cBuffer::cbChangeOnResize cbCOR ;
	cbCOR.mProjection = XMMatrixTranspose(XMMatrixPerspectiveFovLH( XM_PIDIV4, 
											static_cast<float>(this->window.width) / static_cast<float>(this->window.height), 
											0.01f, 100000.0f ));

	pImmediateContext->UpdateSubresource( pCBNeverChanges, 0, NULL, &cbCNV, 0, 0 );
	pImmediateContext->UpdateSubresource( pCBChangesOnResize, 0, NULL, &cbCOR, 0, 0 );
		
	pImmediateContext->VSSetConstantBuffers( 0, 1, &pCBNeverChanges );
	pImmediateContext->VSSetConstantBuffers( 1, 1, &pCBChangesOnResize );

	for(auto objectIter = this->objects.begin();
		objectIter != this->objects.end();
		++objectIter)
	{
		(*objectIter)->Draw();
	}

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
						
			//Sleep( 10 - timer._frameTime);
			for(auto objectIter = this->objects.begin();
				objectIter != this->objects.end();
				++objectIter)
			{
				(*objectIter)->Update(static_cast<float>(timer._absoluteTime));
			}

			// render
			this->Render();

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
		
	this->CBNeverChangesID      = "CBNeverChange";
	this->CBChangesOnResizeID   = "CBChangesOnResize";

	ID3D11Device* device = ((DX11App*)App::getInstance())->direct3d.pd3dDevice;	
	std::wstring error;

	if(!DX11ObjectManager::getInstance()->CBuffer.Exists(this->CBNeverChangesID))
	{
		ID3D11Buffer* constantBuffer;
		if(!cBuffer::LoadBuffer<cBuffer::cbNeverChanges>(device, &constantBuffer, error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->CBuffer.Add(this->CBNeverChangesID, constantBuffer);
	}

	if(!DX11ObjectManager::getInstance()->CBuffer.Exists(this->CBChangesOnResizeID))
	{
		ID3D11Buffer* constantBuffer;
		if(!cBuffer::LoadBuffer<cBuffer::cbChangeOnResize>(device, &constantBuffer, error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->CBuffer.Add(this->CBChangesOnResizeID, constantBuffer);
	}

	if(!DX11ObjectManager::getInstance()->Sampler.Exists(this->SamplerLinear))
	{
		ID3D11SamplerState* samplerLinear;
		if(!DX11Helper::LoadSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_COMPARISON_NEVER,
										0.0f,
										D3D11_FLOAT32_MAX, device,
										&samplerLinear,
										error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->Sampler.Add(this->SamplerLinear, samplerLinear);
	}

	if(!DX11ObjectManager::getInstance()->Sampler.Exists(this->SamplerAnisotropic))
	{
		ID3D11SamplerState* samplerAnisotropic;
		if(!DX11Helper::LoadSamplerState(D3D11_FILTER_ANISOTROPIC,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_TEXTURE_ADDRESS_WRAP,
										D3D11_COMPARISON_NEVER,
										0.0f,
										D3D11_FLOAT32_MAX,
										device,
										&samplerAnisotropic,
										error))
		{
			throw std::exception(Helper::WStringtoString(error).c_str());
		}
		DX11ObjectManager::getInstance()->Sampler.Add(this->SamplerAnisotropic, samplerAnisotropic);
	}
	
	for(int i = 0; i < 100; ++i)
	{
		PlyFile* object = new PlyFile(L"PlyFiles/Box.ply");
		object->Init();	
		objects.push_back(object);
	}

	SkyBox* skyBox = new SkyBox("Texture/AboveSea/AboveSea.dds");
	skyBox->Init();	
	objects.push_back(skyBox);

	return true;
}
void Application::CleanupDevices()
{
	DX11App::InitDevices();

	while(this->objects.size() > 0)
	{
		(*this->objects.begin())->Clean();
		this->objects.erase(this->objects.begin());
		
	}

	
	DX11ObjectManager::getInstance()->CBuffer.Erase(this->CBNeverChangesID);
	DX11ObjectManager::getInstance()->CBuffer.Erase(this->CBChangesOnResizeID);
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
App* Application::getInstance()
{
	if(singletonFlag == false)
	{
		app = new Application;
		singletonFlag = true;
	}

    return app;
}
Application::~Application()
{
	this->CleanupDevices();
}