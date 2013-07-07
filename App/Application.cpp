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
#include "HavokPhysics.h"
#include "Shadow.h"
#include "IPhysicsObject.h"
#include "HavokInclude.h"
#include "BoxHavok.h"
#include "WheelConstraint.h"
#include <cmath>
#include <TlHelp32.h>

void Application::Render()
{	
	this->SetupDepthTexture();

	this->ClearScreen();
	this->SetupDraw();
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
void Application::SetupDraw()
{
	this->SetupCBNeverChanges();
	this->SetupCBChangesOnResize();
}
void Application::SetupSampler()
{
	ID3D11DeviceContext* pImmediateContext = this->direct3d.pImmediateContext;

	pImmediateContext->PSSetSamplers( 0, 1, &(this->pSamplerLinear.second) );
	pImmediateContext->PSSetSamplers( 1, 1, &(this->pSamplerAnisotropic.second) );
}
void Application::SetupCBNeverChanges()
{
	ID3D11DeviceContext* pImmediateContext = this->direct3d.pImmediateContext;
	XMFLOAT4X4 cameraView      = this->camera.GetViewMatrix();	
	XMFLOAT4X4 lightView       = this->lightManager.GetViewMatrix(0);

	cBuffer::cbNeverChanges cbCNV ;
	if(this->DrawingShadow) { cbCNV.mCameraView = XMMatrixTranspose(XMLoadFloat4x4(&lightView)); }
	else					{ cbCNV.mCameraView = XMMatrixTranspose(XMLoadFloat4x4(&cameraView)); }	
	cbCNV.mLightView       = XMMatrixTranspose(XMLoadFloat4x4(&lightView));
	cbCNV.eye              = App::getInstance()->camera.Eye();
	cbCNV.target           = App::getInstance()->camera.Target();
	for(int i = 0; i < 10; ++i)
	{
		cbCNV.lights[i] = this->lightManager.GetLightBuffer(i);
	}

	pImmediateContext->UpdateSubresource( this->pCBNeverChangesID.second, 0, NULL, &cbCNV, 0, 0 );
	pImmediateContext->VSSetConstantBuffers( 0, 1, &this->pCBNeverChangesID.second );
	pImmediateContext->PSSetConstantBuffers( 0, 1, &this->pCBNeverChangesID.second );
}
void Application::SetupCBChangesOnResize()
{
	ID3D11DeviceContext* pImmediateContext = this->direct3d.pImmediateContext;
	
	XMFLOAT4X4 projection = Projection.GetPrespective();

	auto old = Projection;
	Projection.SetMinViewable(1.0f);
	Projection.SetMaxViewable(5000.0f);
	Projection.SetFovAngle(XM_PIDIV2);
	XMFLOAT4X4 lightProjection = Projection.GetPrespective();
	Projection = old;
	
	cBuffer::cbChangeOnResize cbCOR ;
	if(this->DrawingShadow) { cbCOR.mProjection = XMMatrixTranspose(XMLoadFloat4x4(&lightProjection)); }
	else					{ cbCOR.mProjection = XMMatrixTranspose(XMLoadFloat4x4(&projection)); }	
	
	cbCOR.mLightProjection = XMMatrixTranspose(XMLoadFloat4x4(&lightProjection));

	pImmediateContext->UpdateSubresource( this->pCBChangesOnResizeID.second, 0, NULL, &cbCOR, 0, 0 );
	pImmediateContext->VSSetConstantBuffers( 1, 1, &this->pCBChangesOnResizeID.second );
	pImmediateContext->PSSetConstantBuffers( 1, 1, &this->pCBChangesOnResizeID.second );
}
void Application::SetupDepthTexture()
{	
	this->DrawingShadow = true;
	this->SetupDraw();
	Shadow::CreateShadow();
	this->DrawingShadow = false;
}
void Application::DrawObjects()
{
	std::multimap<float, iObjectDrawable*, std::greater<float>> sortedObjects;

	for(auto objectIter = this->objects.begin();
		objectIter != this->objects.end();
		++objectIter)
	{
		if(objectIter->second.DrawNext)
		{
			sortedObjects.insert(std::pair<float, iObjectDrawable*>(objectIter->second.Sort, objectIter->second.ObjectDrawable));
		}
	}

	for(auto iter = sortedObjects.begin();
		iter != sortedObjects.end();
		++iter)
	{
		if(this->DrawingShadow == false)		{ iter->second->Draw(); }
		else if(this->DrawingShadow == true)	{ iter->second->DrawDepth(); }
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
			this->timer._sinceLastDraw += timer._frameTime;
			this->timer._sinceLastPhysicUpdate += timer._frameTime;
			this->timer._sinceLastInputUpdate += timer._frameTime;

			if(this->timer._sinceLastPhysicUpdate > 0.001)
			{
				HavokPhysics::getInstance()->Update(0.001f);
				this->timer._sinceLastPhysicUpdate = 0.0;
			}

			if(this->timer._sinceLastInputUpdate > 0.001)
			{
				for(auto iter = this->objects.begin();
					iter != this->objects.end();
					++iter)
				{
					iter->second.Update(static_cast<float>(0.001f));
				}
				this->timer._sinceLastInputUpdate = 0.0;
			}
			this->SortObject();

			if(this->timer._sinceLastDraw > 0.012)
			{
				// render
				this->Render();
				this->Present();
				this->timer._sinceLastDraw = 0.0;
			}

			std::vector<std::string> toDelete;

			for(auto iter = this->objects.begin();
				iter != this->objects.end();
				++iter)
			{
				if(iter->second.Delete == true)
				{
					toDelete.push_back(iter->first);
				}
			}

			for(std::size_t i = 0; i < toDelete.size(); ++i)
			{
				auto loc = this->objects.find(toDelete[i]);
				this->objects.erase(loc);
			}

			// update fps
			timerLast = timerNow;
			++(timer._frameCount);
		}
	}

	KillTimer( this->window.hWnd, FRAMERATE_UPDATE_TIMER );
}
void Application::InitDevices()
{
	DX11App::InitDevices();
		
	this->DrawingShadow = false;
		
	this->pCBNeverChangesID.first		= "CBNeverChange";
	this->pCBChangesOnResizeID.first	= "CBChangesOnResize";
	this->pSamplerLinear.first			= "SamplerLinear";
	this->pSamplerAnisotropic.first		= "SamplerAnisotropic";

	ID3D11Device* device = DX11App::getInstance()->direct3d.pd3dDevice;	
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
	this->SetupSampler();

	HavokPhysics::getInstance()->Init();
	
	ObjectLoader::getInstance()->LoadXMLFile("Commands.xml");

	this->objects = ObjectLoader::getInstance()->SpawnAll();
	this->lightManager = ObjectLoader::getInstance()->SetupLight();
	ObjectLoader::getInstance()->SetupConstraint();
}
LRESULT Application::CB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static XMFLOAT4 rotationA = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	static XMFLOAT4 rotationB = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	switch( message )
    {

		case WM_KEYUP:
		{
			for(auto iter = this->objects.begin();
				iter != this->objects.end();
				++iter)
			{
				if(iter->second.InputObject != 0)
				{
					iter->second.InputObject->KeyChange(wParam, false);
				}
			}
			break;
		}
		case WM_KEYDOWN: 
		{
			for(auto iter = this->objects.begin();
				iter != this->objects.end();
				++iter)
			{
				if(iter->second.InputObject != 0)
				{
					iter->second.InputObject->KeyChange(wParam, true);
				}
			}

            switch (wParam) 
            { 
                case VK_LEFT: 
					camera.Yaw(-0.1f);
                    return 0;
				case VK_RIGHT:
					camera.Yaw(+0.1f);
					return 0;
				case VK_UP:
					camera.MoveForward(0.5f);
					return 0;
				case VK_DOWN:
					camera.MoveForward(-0.5f);
					return 0;
				case VK_NEXT:
					camera.Pitch(0.1f);
					return 0;
				case VK_PRIOR:
					camera.Pitch(-0.1f);
					return 0;
				case VK_NUMPAD8:
					{
						this->lightManager[0].pos.x += 1.0f;	
					}
					return 0;
				case VK_NUMPAD5:
					{
						this->lightManager[0].pos.x -= 1.0f;	
					}
					return 0;
				case VK_NUMPAD4:
					{
						this->lightManager[0].pos.z -= 1.0f;	
					}
					return 0;
				case VK_NUMPAD6:
					{
						this->lightManager[0].pos.z += 1.0f;	
					}
					return 0;
				case VK_NUMPAD7:
					{
						this->lightManager[0].pos.y += 1.0f;	
					}
					return 0;
				case VK_NUMPAD9:
					{
						this->lightManager[0].pos.y -= 1.0f;	
					}
					return 0;
				case 'W':
					{
						auto objectIter = this->objects.find("Wheel1");
						if(objectIter != this->objects.end())
						{
							hkVector4 torque;

							auto wheelConstraint = dynamic_cast<WheelConstraint*>(HavokPhysics::getInstance()->ConstraintList["ConstraintWheel1"]);

							auto m_wheelRigidBody = dynamic_cast<HavokObject*>(objectIter->second.Physics)->m_RigidBody;

							auto m_wheelConstraint = wheelConstraint->m_wheelConstraint;

							torque.setRotatedDir( m_wheelRigidBody->getRotation(), m_wheelConstraint->m_atoms.m_steeringBase.m_rotationA.getColumn(hkpWheelConstraintData::Atoms::AXIS_AXLE) ); 
							torque.setMul4( -20.0f, torque ); 
							torque.mul4( 10.0f );
							m_wheelRigidBody->applyAngularImpulse( torque );
						}

						objectIter = this->objects.find("Wheel2");
						if(objectIter != this->objects.end())
						{
							hkVector4 torque;

							auto wheelConstraint = dynamic_cast<WheelConstraint*>(HavokPhysics::getInstance()->ConstraintList["ConstraintWheel2"]);

							auto m_wheelRigidBody = dynamic_cast<HavokObject*>(objectIter->second.Physics)->m_RigidBody;

							auto m_wheelConstraint = wheelConstraint->m_wheelConstraint;

							torque.setRotatedDir( m_wheelRigidBody->getRotation(), m_wheelConstraint->m_atoms.m_steeringBase.m_rotationA.getColumn(hkpWheelConstraintData::Atoms::AXIS_AXLE) ); 
							torque.setMul4( -20.0f, torque ); 
							torque.mul4( 10.0f );
							m_wheelRigidBody->applyAngularImpulse( torque );
						}

						objectIter = this->objects.find("Wheel3");
						if(objectIter != this->objects.end())
						{
							hkVector4 torque;

							auto wheelConstraint = dynamic_cast<WheelConstraint*>(HavokPhysics::getInstance()->ConstraintList["ConstraintWheel3"]);

							auto m_wheelRigidBody = dynamic_cast<HavokObject*>(objectIter->second.Physics)->m_RigidBody;

							auto m_wheelConstraint = wheelConstraint->m_wheelConstraint;

							torque.setRotatedDir( m_wheelRigidBody->getRotation(), m_wheelConstraint->m_atoms.m_steeringBase.m_rotationA.getColumn(hkpWheelConstraintData::Atoms::AXIS_AXLE) ); 
							torque.setMul4( -20.0f, torque ); 
							torque.mul4( 10.0f );
							m_wheelRigidBody->applyAngularImpulse( torque );
						}

						objectIter = this->objects.find("Wheel4");
						if(objectIter != this->objects.end())
						{
							hkVector4 torque;

							auto wheelConstraint = dynamic_cast<WheelConstraint*>(HavokPhysics::getInstance()->ConstraintList["ConstraintWheel4"]);

							auto m_wheelRigidBody = dynamic_cast<HavokObject*>(objectIter->second.Physics)->m_RigidBody;

							auto m_wheelConstraint = wheelConstraint->m_wheelConstraint;

							torque.setRotatedDir( m_wheelRigidBody->getRotation(), m_wheelConstraint->m_atoms.m_steeringBase.m_rotationA.getColumn(hkpWheelConstraintData::Atoms::AXIS_AXLE) ); 
							torque.setMul4( -20.0f, torque ); 
							torque.mul4( 10.0f );
							m_wheelRigidBody->applyAngularImpulse( torque );
						}
					}
					return 0;
				case 'Q':
					{
						auto objectIter = this->objects.find("Box2");
						if(objectIter != this->objects.end())
						{
							rotationA.x += 0.1f;
							hkQuaternion h;
							h.setFromEulerAngles(rotationA.x, rotationA.y, rotationA.z);
							BoxHavok* box = dynamic_cast<BoxHavok*>(objectIter->second.Physics);
							box->m_RigidBody->setRotation(h);
						}
					}
					return 0;
				case 'E':
					{
						auto objectIter = this->objects.find("Box2");
						if(objectIter != this->objects.end())
						{
							rotationA.x -= 0.1f;
							hkQuaternion h;
							h.setFromEulerAngles(rotationA.x, rotationA.y, rotationA.z);
							BoxHavok* box = dynamic_cast<BoxHavok*>(objectIter->second.Physics);
							box->m_RigidBody->setRotation(h);
						}
					}
					return 0;
				case 'Z': case 'z':
					{
						auto sniperIter = this->objects.find("Sniper");
						if(sniperIter ==this->objects.end())
						{
							Sniper* sniper = new Sniper;
							sniper->Init();
							this->objects["Sniper"].ObjectDrawable = (sniper);
						}
						else
						{
							this->objects.erase(sniperIter);
						}
					}
					return 0;
				case '1':
					{
						auto objectIter = this->objects.find("FBXObject1");
						if(objectIter != this->objects.end())
						{
							FBXObject* fbx = dynamic_cast<FBXObject*>(objectIter->second.ObjectDrawable);
							if(fbx != 0)
							{
								fbx->PlayPartialAnimation("Idle");
							}
						}
					}
					break;
				case '2':
					{
						auto objectIter = this->objects.find("FBXObject1");
						if(objectIter != this->objects.end())
						{
							FBXObject* fbx = dynamic_cast<FBXObject*>(objectIter->second.ObjectDrawable);
							if(fbx != 0)
							{
								fbx->AnimController.partialAnim.play = false;
							}
						}
					}
					break;
				case VK_SUBTRACT:
					{
						auto objectIter = this->objects.find("FBXObject1");
						if(objectIter != this->objects.end())
						{
							FBXObject* fbx = dynamic_cast<FBXObject*>(objectIter->second.ObjectDrawable);
							if(fbx != 0)
							{
								fbx->SetAnimRate(fbx->GetAnimRate() - 0.05f);
							}
						}
					}
					break;
				case VK_ADD:
					{
						auto objectIter = this->objects.find("FBXObject1");
						if(objectIter != this->objects.end())
						{
							FBXObject* fbx = dynamic_cast<FBXObject*>(objectIter->second.ObjectDrawable);
							if(fbx != 0)
							{
								fbx->SetAnimRate(fbx->GetAnimRate() + 0.05f);
							}
						}
					}
					break;
				case VK_F1:
					{
						ObjectLoader::getInstance()->LoadXMLFile("Commands.xml");

						this->objects = ObjectLoader::getInstance()->SpawnAll();
						this->lightManager = ObjectLoader::getInstance()->SetupLight();
						ObjectLoader::getInstance()->SetupConstraint();
					}
				case VK_F2:
					{
						DX11ObjectManager::getInstance()->PlyBuffer.mapObject.clear();
						DX11ObjectManager::getInstance()->IndexBuffer.mapObject.clear();
						DX11ObjectManager::getInstance()->CBuffer.mapObject.clear();
						DX11ObjectManager::getInstance()->InputLayout.mapObject.clear();
						DX11ObjectManager::getInstance()->VertexShader.mapObject.clear();
						DX11ObjectManager::getInstance()->PixelShader.mapObject.clear();
						DX11ObjectManager::getInstance()->RastersizerState.mapObject.clear();
						DX11ObjectManager::getInstance()->Textexture.mapObject.clear();
						DX11ObjectManager::getInstance()->Sampler.mapObject.clear();
						DX11ObjectManager::getInstance()->BelnderState.mapObject.clear();

						ObjectLoader::getInstance()->LoadXMLFile("Commands.xml");

						this->objects = ObjectLoader::getInstance()->SpawnAll();
						this->lightManager = ObjectLoader::getInstance()->SetupLight();
						ObjectLoader::getInstance()->SetupConstraint();
					}
					break;
				case VK_F3:
					{
						HavokPhysics::getInstance()->SetupVisualDebugger();

						HANDLE SnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

						if( SnapShot == INVALID_HANDLE_VALUE )
							return 0;

						PROCESSENTRY32 procEntry;
						procEntry.dwSize = sizeof( PROCESSENTRY32 );

						if( !Process32First( SnapShot, &procEntry ) )
							return 0;

						do
						{
							if( std::wstring(procEntry.szExeFile) == L"hkVisualDebugger.exe" )
							{
								return 0;
							}
						}
						while( Process32Next( SnapShot, &procEntry ) );


						STARTUPINFO info={sizeof(info)};
						PROCESS_INFORMATION processInfo;
						LPTSTR applicationName = L"D:\\hk2012_1_0_r1\\Tools\\VisualDebugger\\hkVisualDebugger.exe";
						LPTSTR cmd = L"localhost";
						CreateProcessW(applicationName, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
						
						applicationName = L"C:\\hk2012_1_0_r1\\Tools\\VisualDebugger\\hkVisualDebugger.exe";
						CreateProcessW(applicationName, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
					}
					break;
				case VK_ESCAPE:
					
					DestroyWindow(this->window.hWnd);
					break;
			}
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

					/*auto objectIter = this->objects.find("FBXObject1");
					if(objectIter != this->objects.end())
					{
						FBXObject* fbx = dynamic_cast<FBXObject*>(objectIter->second.ObjectDrawable);
						if(fbx != 0)
						{
							caption << fbx->GetCurrentAnimFrame();
						}
					}*/

					SetWindowTextW( hWnd, caption.str().c_str() );
					break;
				}
			}
			return 0;
		}
		case WM_DESTROY:
		{
			HANDLE SnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

			if( SnapShot == INVALID_HANDLE_VALUE )
				break;

			PROCESSENTRY32 procEntry;
			procEntry.dwSize = sizeof( PROCESSENTRY32 );

			if( !Process32First( SnapShot, &procEntry ) )
				break;

			do
			{
				if( std::wstring(procEntry.szExeFile) == L"hkVisualDebugger.exe" )
				{
					HANDLE h =  OpenProcess( PROCESS_ALL_ACCESS, false,  _In_  procEntry.th32ProcessID);
					TerminateProcess(h, 0);
				}
			}
			while( Process32Next( SnapShot, &procEntry ) );

		}
		break;
	}
	return DX11App::CB_WndProc(hWnd, message, wParam, lParam);
}	
void Application::CleanupDevices()
{
	DX11App::InitDevices();

	for(auto iter = this->objects.begin();
		iter != this->objects.end();
		++iter)
	{
		iter->second.ObjectDrawable->Clean();
		delete (iter->second.ObjectDrawable);	
	}
}
void Application::LoadD3DStuff()
{
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->pCBNeverChangesID.first, this->pCBNeverChangesID.second)){ throw std::exception("CB never Changes not found"); }
	if(!DX11ObjectManager::getInstance()->CBuffer.Get(this->pCBChangesOnResizeID.first, this->pCBChangesOnResizeID.second)){ throw std::exception("CB Changes on Resize not found"); }
	if(!DX11ObjectManager::getInstance()->Sampler.Get(this->pSamplerLinear.first, this->pSamplerLinear.second)){ throw std::exception("Sampler Linear not found"); }
	if(!DX11ObjectManager::getInstance()->Sampler.Get(this->pSamplerAnisotropic.first, this->pSamplerAnisotropic.second)){ throw std::exception("Sampler Anisotropic not found"); }
}
void Application::SortObject()
{
	for(auto objectIter = this->objects.begin();
		objectIter != this->objects.end();
		++objectIter)
	{
		objectIter->second.Sort = objectIter->second.ObjectDrawable->GetOrder();
	}
}
Application* Application::getInstance()
{
	if(App::app == 0)
	{
		app = new Application;
	}

	Application* appCasted = dynamic_cast<Application*>(app);
	
	if(appCasted == 0)
	{
		throw std::exception("Error: app was not created as an application");
	}

    return appCasted;
}
Application::~Application()
{
}