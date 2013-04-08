#ifndef __iObjectDrawable__
#define __iObjectDrawable__


#include "DrawableInfo.h"
#include <string>
#include <vector>

class iObjectDrawable
{
public:
	virtual void Init()					    = 0;
	virtual void Clean()				    = 0;
	virtual void UpdateDrawing(float delta)	= 0;
	virtual void UpdateObject(float delta)	= 0;
	virtual void Draw()					    = 0;
	virtual void DrawDepth()			    = 0;
	virtual float GetOrder(){return 100000.0f;};
	virtual iObjectDrawable* clone() const	= 0;
	DrawableInfo object;

	virtual ~iObjectDrawable(){}
};


#endif //__iObjectDrawable__