#ifndef _IPhysics_HG_
#define _IPhysics_HG_

class IPhysics
{
public:
	virtual void Init(void) = 0;	
	virtual void CleanUp(void) = 0;
	virtual void Update(float delta) = 0;

	IPhysics* IPhysics::getInstance();
protected:
	static IPhysics* physicsInstance;
};

#endif