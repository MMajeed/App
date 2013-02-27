#include "Helper.h"
#include <sstream>
#include <algorithm>

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
std::string Helper::ToLower(std::string inputString)
{
	std::transform(inputString.begin(), inputString.end(), inputString.begin(), ::tolower);
	return inputString;
}
float Helper::StringToFloat(std::string inputString)
{
	float returnValue = std::numeric_limits<float>::min();

	std::stringstream ss;
	ss << inputString;
	ss >> returnValue;

	return returnValue;	
}