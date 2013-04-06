#include "HavokPhysics.h"

#include <exception>
#include <string>
#include <sstream>

// Math and base include
//#include <Common/Base/hkBase.h>
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

#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>			
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>			

#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>	

#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
#include <Common/Base/Thread/Job/ThreadPool/Spu/hkSpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

//Constraints
#include <Physics/Dynamics/Constraint/Bilateral/BallAndSocket/hkpBallAndSocketConstraintData.h>


// Visual Debugger includes
//#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>		

// Platform specific initialization
#include <Common/Base/System/Init/PlatformInit.cxx>

// This excludes libraries that are not going to be linked
// from the project configuration, even if the keycodes are
// present
// Keycode
#include <Common/Base/keycode.cxx>
#undef HK_FEATURE_PRODUCT_AI
#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#define HK_FEATURE_REFLECTION_PHYSICS
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#define HK_EXCLUDE_FEATURE_MemoryTracker
#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
#define HK_EXCLUDE_FEATURE_RegisterVersionPatches 
#define HK_EXCLUDE_LIBRARY_hkGeometryUtilities
#include <Common/Base/Config/hkProductFeatures.cxx>
#if !defined HK_FEATURE_PRODUCT_PHYSICS
#error Physics is needed to build this demo. It is included in the common package for reference only.
#endif

void HavokPhysics::Init(void)
{
	// Shamelessly stolen from the Havok Physics demo...
	PlatformInit();

	// Need to have memory allocated for the solver. Allocate 1mb for it.
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo( 500* 1024 ) );
	hkBaseSystem::init( memoryRouter, HavokPhysics::ErrorReport );
	
	// The world cinfo contains global simulation parameters, including gravity, solver settings etc.
	hkpWorldCinfo worldInfo;

	// Set the simulation type of the world to multi-threaded.
	worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;

	// Flag objects that fall "out of the world" to be automatically removed - just necessary for this physics scene
	worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;

	this->m_pHavok_world = new hkpWorld(worldInfo);

	// Disable deactivation, so that you can view timers in the VDB. This should not be done in your game.
	this->m_pHavok_world->m_wantDeactivation = false;


	// When the simulation type is SIMULATION_TYPE_MULTITHREADED, in the debug build, the sdk performs checks
	// to make sure only one thread is modifying the world at once to prevent multithreaded bugs. Each thread
	// must call markForRead / markForWrite before it modifies the world to enable these checks.
	this->m_pHavok_world->markForWrite();


	// Register all collision agents, even though only box - box will be used in this particular example.
	// It's important to register collision agents before adding any entities to the world.
	hkpAgentRegisterUtil::registerAllAgents( this->m_pHavok_world->getCollisionDispatcher() );


	#if defined(_DEBUG)
	// Initialize the VDB
	hkArray<hkProcessContext*> contexts;

	// <PHYSICS-ONLY>: Register physics specific visual debugger processes
	// By default the VDB will show debug points and lines, however some products such as physics and cloth have additional viewers
	// that can show geometries etc and can be enabled and disabled by the VDB app.
	hkpPhysicsContext* context;
	{
		// The visual debugger so we can connect remotely to the simulation
		// The context must exist beyond the use of the VDB instance, and you can make
		// whatever contexts you like for your own viewer types.
		context = new hkpPhysicsContext();
		hkpPhysicsContext::registerAllPhysicsProcesses(); // all the physics viewers
		context->addWorld(this->m_pHavok_world); // add the physics world so the viewers can see it
		contexts.pushBack(context);

		// Now we have finished modifying the world, release our write marker.
		this->m_pHavok_world->unmarkForWrite();
	}

	this->m_pVisualDebugger = new hkVisualDebugger(contexts);
	this->m_pVisualDebugger->serve();
	#endif
}
void HavokPhysics::CleanUp(void)
{

}
void HavokPhysics::Update(float delta)
{
	this->m_pHavok_world->stepDeltaTime(delta);
	#if defined(_DEBUG)
	this->m_pVisualDebugger->step();
	#endif
}

void HK_CALL HavokPhysics::ErrorReport(const char* msg, void* userContext)
{
	std::stringstream ss;
	ss << "An error occured in Havok: " << msg;
	throw std::exception(ss.str().c_str());
}

HavokPhysics* HavokPhysics::getInstance()
{
	if(HavokPhysics::physicsInstance == 0)
	{
		HavokPhysics::physicsInstance = new HavokPhysics();
	}

	HavokPhysics* physicCasted = dynamic_cast<HavokPhysics*>(HavokPhysics::physicsInstance);
	
	if(physicCasted == 0)
	{
		throw std::exception("Error: app was not created as an application");
	}

	return physicCasted;
}