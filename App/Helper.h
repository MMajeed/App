#ifndef __HelperFunc__
#define __HelperFunc__

#include <string>

class Helper
{
public:
	static std::string WStringtoString( std::wstring inputString );
	static std::wstring stringToWstring( std::string inputString );
	static std::string ToLower(std::string inputString);
	static float StringToFloat(std::string inputString);
	static int StringToInt(std::string inputString);
};

#endif //__HelperFunc__