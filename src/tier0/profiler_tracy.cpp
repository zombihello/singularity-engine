#include "pch_tier0.h"

#if ENABLE_PROFILING && PLATFORM_USE_TRACY_PROFILER
	#include <EASTL/atomic.h>
	#include <tracy/tracy/Tracy.hpp>
	#include <tracy/common/TracyColor.hpp>

	#include "tier0/iprofiler.h"

	#define TRACY_CALLSTACK_DEPTH 32

static const uint32 s_profilerScopeGroupColors[] = {
	tracy::Color::DarkGray,		  // PROFILER_SCOPE_GROUP_NONE
	tracy::Color::Purple,		  // PROFILER_SCOPE_GROUP_AI
	tracy::Color::LightSkyBlue,	  // PROFILER_SCOPE_GROUP_ANIMATION
	tracy::Color::HotPink,		  // PROFILER_SCOPE_GROUP_AUDIO
	tracy::Color::Black,		  // PROFILER_SCOPE_GROUP_DEBUG
	tracy::Color::Black,		  // PROFILER_SCOPE_GROUP_CAMERA
	tracy::Color::DarkGreen,	  // PROFILER_SCOPE_GROUP_CLOTH
	tracy::Color::RoyalBlue,	  // PROFILER_SCOPE_GROUP_GAMELOGIC
	tracy::Color::Ivory,		  // PROFILER_SCOPE_GROUP_INPUT
	tracy::Color::Magenta,		  // PROFILER_SCOPE_GROUP_NAVIGATION
	tracy::Color::Olive,		  // PROFILER_SCOPE_GROUP_NETWORK
	tracy::Color::LawnGreen,	  // PROFILER_SCOPE_GROUP_PHYSICS
	tracy::Color::Burlywood,	  // PROFILER_SCOPE_GROUP_RENDERING
	tracy::Color::RoyalBlue,	  // PROFILER_SCOPE_GROUP_SCENE
	tracy::Color::Plum,			  // PROFILER_SCOPE_GROUP_SCRIPT
	tracy::Color::Gold,			  // PROFILER_SCOPE_GROUP_STREAMING
	tracy::Color::PaleTurquoise,  // PROFILER_SCOPE_GROUP_UI
	tracy::Color::SaddleBrown,	  // PROFILER_SCOPE_GROUP_VFX
	tracy::Color::Snow,			  // PROFILER_SCOPE_GROUP_VISIBILITY
	tracy::Color::Tomato,		  // PROFILER_SCOPE_GROUP_WAIT
	tracy::Color::Khaki			  // PROFILER_SCOPE_GROUP_IO
};
static_assert( ARRAYSIZE( s_profilerScopeGroupColors ) == PROFILER_SCOPE_NUM_GROUPS, "Array size 's_profilerScopeGroupColors' must be equal to PROFILER_SCOPE_NUM_GROUPS" );

//-----------------------------------------------------------------------------
// Tracy profiler
//-----------------------------------------------------------------------------
class CProfilerTracy : public IProfiler
{
public:
	// IProfiler interface
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void Update() override;

	virtual void RegisterScope( profilerScopeData_t& scopeData ) override;
	virtual void BeginScope( const profilerScopeData_t& scopeData, const char* pScopeName = NULL ) override;
	virtual void EndScope( const profilerScopeData_t& scopeData ) override;

	virtual void BeginThreadScope( const char* pThreadName ) override;
	virtual void EndThreadScope() override;

	virtual void BeginMarkScope( const char* pMarkName ) override;
	virtual void EndMarkScope( const char* pMarkName ) override;
	virtual void NextFrame() override;

	virtual void SendMessage( const char* pMessage ) override;
	virtual void SendMemAlloc( const void* pPtr, size numBytes, const char* pAllocatorName = NULL ) override;
	virtual void SendMemFree( const void* pPtr, const char* pAllocatorName = NULL ) override;

	virtual bool IsInited() const override;
	virtual bool IsConnected() const override;

	CProfilerTracy();

private:
	bool				  bInitialized;
	eastl::atomic<uint32> activeScopeCount;
};

/*
==================
CProfilerTracy::CProfilerTracy
==================
*/
CProfilerTracy::CProfilerTracy()
	: bInitialized( false )
{
	activeScopeCount.store( 0, eastl::memory_order_release );
}

/*
==================
CProfilerTracy::Init
==================
*/
void CProfilerTracy::Init()
{
	bInitialized = true;
}

/*
==================
CProfilerTracy::Shutdown
==================
*/
void CProfilerTracy::Shutdown()
{
	bInitialized = false;
}

/*
==================
CProfilerTracy::Update
==================
*/
void CProfilerTracy::Update()
{
}

/*
==================
CProfilerTracy::RegisterScope
==================
*/
void CProfilerTracy::RegisterScope( profilerScopeData_t& scopeData )
{
	scopeData.id = activeScopeCount.load( eastl::memory_order_relaxed );
	activeScopeCount.fetch_add( 1, eastl::memory_order_relaxed );
}

/*
==================
CProfilerTracy::BeginScope
==================
*/
void CProfilerTracy::BeginScope( const profilerScopeData_t& scopeData, const char* pScopeName /* = NULL */ )
{
	// Tracy begin the scope
	{
		TracyQueuePrepare( tracy::QueueType::ZoneBeginAllocSrcLoc );
		uint64 srcLoc = tracy::Profiler::AllocSourceLocation( scopeData.line, scopeData.pFileName, S_Strlen( scopeData.pFileName ), scopeData.pFunctionName, S_Strlen( scopeData.pFunctionName ), scopeData.pScopeName, S_Strlen( scopeData.pScopeName ), 0 );
		tracy::MemWrite( &item->zoneBegin.time, tracy::Profiler::GetTime() );
		tracy::MemWrite( &item->zoneBegin.srcloc, srcLoc );
		TracyQueueCommit( zoneBeginThread );
	}

	// Set a name for the scope
	if ( pScopeName )
	{
		uint32 scopeNameLen	   = S_Strlen( pScopeName );
		char*  pTracyScopeName = (char*)tracy::tracy_malloc( scopeNameLen );
		Mem_Memcpy( pTracyScopeName, pScopeName, scopeNameLen );

		TracyQueuePrepare( tracy::QueueType::ZoneName );
		tracy::MemWrite( &item->zoneTextFat.text, (uint64)pTracyScopeName );
		tracy::MemWrite( &item->zoneTextFat.size, (uint16)scopeNameLen );
		TracyQueueCommit( zoneTextFatThread );
	}

	// Set a color for the scope
	{
		uint32 color = s_profilerScopeGroupColors[scopeData.scopeGroup];
		TracyQueuePrepare( tracy::QueueType::ZoneColor );
		tracy::MemWrite( &item->zoneColor.b, uint8( ( color ) & 0xFF ) );
		tracy::MemWrite( &item->zoneColor.g, uint8( ( color >> 8 ) & 0xFF ) );
		tracy::MemWrite( &item->zoneColor.r, uint8( ( color >> 16 ) & 0xFF ) );
		TracyQueueCommit( zoneColorThread );
	}
}

/*
==================
CProfilerTracy::EndScope
==================
*/
void CProfilerTracy::EndScope( const profilerScopeData_t& scopeData )
{
	// Tracy end of the scope
	TracyQueuePrepare( tracy::QueueType::ZoneEnd );
	tracy::MemWrite( &item->zoneEnd.time, tracy::Profiler::GetTime() );
	TracyQueueCommit( zoneEndThread );
}

/*
==================
CProfilerTracy::BeginThreadScope
==================
*/
void CProfilerTracy::BeginThreadScope( const char* pThreadName )
{
	tracy::SetThreadName( pThreadName );
}

/*
==================
CProfilerTracy::EndThreadScope
==================
*/
void CProfilerTracy::EndThreadScope()
{
}

/*
==================
CProfilerTracy::BeginMarkScope
==================
*/
void CProfilerTracy::BeginMarkScope( const char* pMarkName )
{
	FrameMarkStart( pMarkName );
}

/*
==================
CProfilerTracy::EndMarkScope
==================
*/
void CProfilerTracy::EndMarkScope( const char* pMarkName )
{
	FrameMarkEnd( pMarkName );
}

/*
==================
CProfilerTracy::NextFrame
==================
*/
void CProfilerTracy::NextFrame()
{
	FrameMark;
}

/*
==================
CProfilerTracy::SendMessage
==================
*/
void CProfilerTracy::SendMessage( const char* pMessage )
{
	TracyMessage( pMessage, S_Strlen( pMessage ) );
}

/*
==================
CProfilerTracy::SendMemAlloc
==================
*/
void CProfilerTracy::SendMemAlloc( const void* pPtr, size numBytes, const char* pAllocatorName /* = NULL */ )
{
	if ( pAllocatorName )
	{
		TracyAllocNS( pPtr, numBytes, TRACY_CALLSTACK_DEPTH, pAllocatorName );
	}
	else
	{
		TracyAllocS( pPtr, numBytes, TRACY_CALLSTACK_DEPTH );
	}
}

/*
==================
CProfilerTracy::SendMemFree
==================
*/
void CProfilerTracy::SendMemFree( const void* pPtr, const char* pAllocatorName /* = NULL */ )
{
	if ( pAllocatorName )
	{
		TracyFreeNS( pPtr, TRACY_CALLSTACK_DEPTH, pAllocatorName );
	}
	else
	{
		TracyFreeS( pPtr, TRACY_CALLSTACK_DEPTH );
	}
}

/*
==================
CProfilerTracy::IsInited
==================
*/
bool CProfilerTracy::IsInited() const
{
	return bInitialized;
}

/*
==================
CProfilerTracy::IsConnected
==================
*/
bool CProfilerTracy::IsConnected() const
{
	return TracyIsConnected;
}

/*
==================
Profiler
==================
*/
IProfiler* Profiler()
{
	static CProfilerTracy s_profilerTracy;
	return &s_profilerTracy;
}
#endif	// ENABLE_PROFILING && PLATFORM_USE_TRACY
