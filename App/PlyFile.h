#ifndef __PlyFile__
#define __PlyFile__

#include "Object.h"
#include "BasicObject.h"
#include "DX11Helper.h"
#include "VertexBuffer.h"
#include <string>
#include <map>

class PlyFile : public BasicObject
{
public:	
	PlyFile(std::wstring plyFileName);
	PlyFile();
	static PlyFile* Spawn(std::map<std::string, std::string> info);
};


#endif //__BasicObject__
