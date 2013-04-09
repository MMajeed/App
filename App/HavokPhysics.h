//#ifndef _HavokPhysics_
//#define _HavokPhysics_
//
//#include "IPhysics.h"
//
//#include <Common/Base/hkBase.h>
//#include <Physics/Dynamics/World/hkpWorld.h>
//#include <Common/Visualize/hkVisualDebugger.h>
//
//class HavokPhysics : public IPhysics
//{
//public:
//	virtual void Init(void);	
//	virtual void CleanUp(void);
//	virtual void Update(float delta);
//	
//	static void HK_CALL ErrorReport(const char* msg, void* userContext);
//
//	hkpWorld*			m_pHavok_world;
//	#if defined(_DEBUG)
//	hkVisualDebugger*	m_pVisualDebugger;
//	#endif
//
//	static HavokPhysics* getInstance();
//};
//
//#endif //_HavokPhysics_