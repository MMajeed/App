#ifndef __iObjectDrawable__
#define __iObjectDrawable__

#include <string>
#include <vector>
class iObjectDrawable
{
public:
	virtual void Init()					= 0;
	virtual void Clean()				= 0;
	virtual void Update(float delta)	= 0;
	virtual void Draw()					= 0;

	virtual ~iObjectDrawable(){}
};


#endif //__iObjectDrawable__