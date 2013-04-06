#ifndef _IPHysicsObject_HG_
#define _IPHysicsObject_HG_

#include <string>
#include <vector>

class IPhysicsObject
{
public:
	virtual void Init(void) = 0;	
	virtual void shutDown(void) = 0;

};

#endif //_IPHysicsObject_HG_