#ifndef _HavokInclude_
#define _HavokInclude_

#include <Common/Base/hkBase.h>
#include <Physics/Dynamics/World/hkpWorld.h>
#include <Common/Visualize/hkVisualDebugger.h>

// Math and base include
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>

// Dynamics includes
#include <Physics/Collide/hkpCollide.h>										
#include <Physics/Collide/Agent/ConvexAgent/SphereBox/hkpSphereBoxAgent.h>	
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>					
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>				
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>					
#include <Physics/Collide/Shape/Convex/Triangle/hkpTriangleShape.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Common\Base\Types\Geometry/hkStridedVertices.h> 

#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>			
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>			

#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>	

#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
#include <Common/Base/Thread/Job/ThreadPool/Spu/hkSpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

//Constraints
#include <Physics/Dynamics/Constraint/Bilateral/BallAndSocket/hkpBallAndSocketConstraintData.h>
#include <Physics/Dynamics/Constraint/Bilateral/Hinge/hkpHingeConstraintData.h> 
#include <Physics/Dynamics/Constraint/Bilateral/Wheel/hkpWheelConstraintData.h> 

// Visual Debugger includes
//#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>		

// Platform specific initialization
#include <Common/Base/System/Init/PlatformInit.cxx>


#include <Physics/Dynamics/Constraint/hkpConstraintData.h>
#include <Physics/Dynamics/Collide/ContactListener/Util/hkpEndOfStepCallbackUtil.h>
#include <Physics/Dynamics/Collide/Filter/Constraint/hkpConstraintCollisionFilter.h>
					
#include <Physics/Dynamics/Entity/hkpRigidBody.h>


#endif _HavokInclude_