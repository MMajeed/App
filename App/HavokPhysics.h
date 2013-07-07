#ifndef _HavokPhysics_
#define _HavokPhysics_

#include "IPhysics.h"

#include <Common/Base/hkBase.h>
#include <Physics/Dynamics/World/hkpWorld.h>
#include <Common/Visualize/hkVisualDebugger.h>
#include <vector>

#include "HavokError.h"

class HavokPhysics : public IPhysics
{
public:
	virtual void Init(void);	
	virtual void CleanUp(void);
	virtual void Update(float delta);
	
	virtual void SetupVisualDebugger();
	virtual void ShutdownVisualDebugger();

	HavokPhysics();

	hkpWorld*			m_pHavok_world;
	hkVisualDebugger*	m_pVisualDebugger;
	HavokError*			m_HavokError;
	bool				m_VisualDebuggerSet;

	virtual void AddConstraint(IPhysicsConstraint* constaint);
	
	static HavokPhysics* getInstance();
	static void HK_CALL ErrorReport(const char* msg, void* userContext);
};

#endif //_HavokPhysics_