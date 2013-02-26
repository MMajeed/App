#ifndef __ObjectDrawable__
#define __ObjectDrawable__

#include "iObjectDrawable.h"

class ObjectDrawable : public iObjectDrawable
{
public:
	virtual void Init();
	virtual void Clean();
	virtual void Update(float delta);
	virtual void Draw();
};


#endif //__ObjectDrawable__