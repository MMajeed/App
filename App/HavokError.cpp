#include "HavokError.h"
#include "Helper.h"
#include <Windows.h>
#include <sstream>

HavokError::HavokError( hkErrorReportFunction errorReportFunction, void* errorReportObject )
	: hkDefaultError( errorReportFunction, errorReportObject )
{}

int HavokError::message(Message m, int id, const char* description, const char* file, int line)
{
	// Output everything except reports to my own error output mechanism, 
	// in this case a custom dialog class.
	switch(m)
	{
	case hkError::MESSAGE_REPORT:
		OutputDebugStringA( description );
		break;
	case hkError::MESSAGE_WARNING:		
		OutputDebugStringA( description );
		break;
	case hkError::MESSAGE_ASSERT:
		{
		std::stringstream ss;
		ss  << "ID = " << id << "\n"
			<< "Type = ERROR" << "\n"
			<< "File = " << file << "\n" 
			<< "Line = " << line << "\n"
			<< "Message = " << description;
		throw std::exception(ss.str().c_str());
		}
		break;
	case hkError::MESSAGE_ERROR:
		{
		std::stringstream ss;
		ss  << "ID = " << id << "\n"
			<< "Type = ERROR" << "\n"
			<< "File = " << file << "\n"
			<< "Line = " << line << "\n"
			<< "Message = " << description;
		throw std::exception(ss.str().c_str());
		}
		break;
	}

	// We return positive for errors and asserts, but not warnings or reports.
	// A positive return value is used by HK_ASSERT and HK_ERROR to force a breakpoint.
	return m == hkError::MESSAGE_ASSERT || m == hkError::MESSAGE_ERROR;
}