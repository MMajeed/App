#include "Helper.h"
#include <sstream>

std::string Helper::WStringtoString( std::wstring inputString )
{
	std::string converted(inputString.begin(), inputString.end());

	return converted;
}
std::wstring Helper::stringToWstring( std::string inputString )
{
	std::wstring converted(inputString.begin(), inputString.end());

	return converted;
}