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
	virtual void Draw()				= 0;	// Calls the three below
	virtual void SetupDraw()			= 0;	// Setups what I'm about to draw
	virtual void SetupTexture()			= 0;	// Setups the texture for what I will draw
	virtual void DrawObject()			= 0;	// Draws the object

	virtual ~iObjectDrawable(){}
};


#endif //__iObjectDrawable__