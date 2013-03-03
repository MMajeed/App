#ifndef __PlyFile__
#define __PlyFile__

#include "BasicObject.h"
#include <string>
#include <map>

class PlyFile : public BasicObject
{
public:	
	PlyFile();
	static PlyFile* Spawn(std::map<std::string, std::string> info);
	virtual float GetOrder();
};


#endif //__PlyFile__
