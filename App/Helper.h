#ifndef __HelperFunc__
#define __HelperFunc__

#include <string>

class Helper
{
public:
	static std::string WStringtoString( std::wstring inputString );
	static std::wstring stringToWstring( std::string inputString );
};

#endif //__HelperFunc__