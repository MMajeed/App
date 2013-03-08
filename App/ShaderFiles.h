#ifndef __ShaderFile__
#define __ShaderFile__

#include <string>

class ShaderFiles
{
public:
	struct ShaderFile
	{
		std::string FileName;
		std::string EntryPoint;
		std::string Mode;
	};

	ShaderFile ShaderInput;
	ShaderFile ShaderVertex;
	ShaderFile ShaderPixel;
};

#endif //__ShaderFile__