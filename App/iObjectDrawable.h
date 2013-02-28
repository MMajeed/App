#ifndef __iObjectDrawable__
#define __iObjectDrawable__


#include "Object.h"
#include <string>
#include <vector>

class iObjectDrawable
{
public:
	virtual void Init()					= 0;
	virtual void Clean()				= 0;
	virtual void Update(float delta)	= 0;
	virtual void Draw()					= 0;	// Calls the three below
	
	Object object;

	virtual ~iObjectDrawable(){}
};


#endif //__iObjectDrawable__