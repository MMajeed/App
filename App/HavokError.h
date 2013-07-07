#ifndef _HavokError_
#define _HavokError_


#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>

class HavokError : public hkDefaultError
{
	public:
    HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
    HavokError( hkErrorReportFunction errorReportFunction, void* errorReportObject = HK_NULL );

    virtual int message(Message m, int id, const char* description, const char* file, int line);
};

#endif //_HavokError_