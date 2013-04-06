#ifndef __PlyFile__
#define __PlyFile__

#include "BasicObject.h"
#include <string>
#include <map>

class PlyFile : public BasicObject
{
public:	
	PlyFile();

	virtual iObjectDrawable* clone() const;

	static PlyFile* Spawn(std::map<std::string, std::string> info);
};


#endif //__PlyFile__
