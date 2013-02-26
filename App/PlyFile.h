#ifndef __PlyFile__
#define __PlyFile__

#include "Object.h"
#include "BasicObject.h"
#include "DX11Helper.h"
#include "VertexBuffer.h"
#include <string>

class PlyFile : public BasicObject
{
public:	
	PlyFile(std::wstring plyFileName);
};


#endif //__BasicObject__