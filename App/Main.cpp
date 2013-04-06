#include <Windows.h>
#include "Application.h"
#include <crtdbg.h>
#include <exception>
#include <sstream>
#include "Helper.h"

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(hPrevInstance);
	//#if defined(_DEBUG)
	//	int dbgFlags = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	//	dbgFlags |= _CRTDBG_CHECK_ALWAYS_DF;	// check block integrity on every memory call
	//	/**	_CRTDBG_DELAY_FREE_MEM_DF will degrade the framerate over time (and kill it in the short term as well). */
	//	dbgFlags |= _CRTDBG_LEAK_CHECK_DF;		// check for memory leak at process termination
	//	_CrtSetDbgFlag( dbgFlags );
	//#endif

	try
	{
		App* theApp = Application::getInstance();
		theApp->Run(hInstance, nCmdShow);
	}
	catch(std::exception ex)
	{
		std::wstringstream wss;
		wss << "Exception: Something went wrong" << std::endl
			<< ex.what();

		OutputDebugStringA( Helper::WStringtoString(wss.str()).c_str() );
		MessageBox( NULL, wss.str().c_str(), L"Exception", MB_ICONERROR );
		return EXIT_FAILURE;
	}
	catch( ... )
	{
		OutputDebugStringA( "Something went wrong" );
		MessageBoxA( NULL, "An unknown exception has been caught" , "Exception", MB_ICONERROR );
		return EXIT_FAILURE;
	}

	return 1;
}