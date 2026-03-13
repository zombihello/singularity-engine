#pragma once
#include "tier0/tier0.h"

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#if ENABLE_PROFILING
	#define INTERNAL_PROFILER_SCOPE_VAR( ScopeName, ScopeGroup )                                                                   \
		static profilerScopeData_t CONCAT( s_autogenScopeData_, __LINE__ )( FUNCTION, FUNCSIG, __FILE__, __LINE__, ##ScopeGroup ); \
		CProfilerScope			   CONCAT( s_autogenScope_, __LINE__ )( CONCAT( s_autogenScopeData_, __LINE__ ), ScopeName );

	#define PROFILER_SCOPE_FUNC()						  INTERNAL_PROFILER_SCOPE_VAR( NULL, PROFILER_SCOPE_GROUP_NONE )
	#define PROFILER_SCOPE_FUNC_GROUP( ScopeGroup )		  INTERNAL_PROFILER_SCOPE_VAR( NULL, ScopeGroup )
	#define PROFILER_SCOPE( ScopeName )					  INTERNAL_PROFILER_SCOPE_VAR( ScopeName, PROFILER_SCOPE_GROUP_NONE )
	#define PROFILER_SCOPE_GROUP( ScopeName, ScopeGroup ) INTERNAL_PROFILER_SCOPE_VAR( ScopeName, ScopeGroup )
	#define PROFILER_THREAD( ThreadName )				  CProfilerThreadScope CONCAT( s_autogenThreadScope_, __LINE__ )( ThreadName )
	#define PROFILER_MARK( MarkName )					  CProfilerMarkScope CONCAT( s_autogenMarkScope_, __LINE__ )( MarkName )
	#define PROFILER_MESSAGE( Message )					  Profiler()->SendMessage( Message )
	#define PROFILER_MEM_ALLOC( Ptr, Size, ... )		  Profiler()->SendMemAlloc( Ptr, Size, ##__VA_ARGS__ )
	#define PROFILER_MEM_FREE( Ptr, ... )				  Profiler()->SendMemFree( Ptr, ##__VA_ARGS__ )
#else
	#define PROFILER_SCOPE_FUNC()
	#define PROFILER_SCOPE_FUNC_GROUP( ScopeGroup )
	#define PROFILER_SCOPE( ScopeName )
	#define PROFILER_SCOPE_GROUP( ScopeName, ScopeGroup )
	#define PROFILER_THREAD( ThreadName )
	#define PROFILER_MARK( MarkName )
	#define PROFILER_MESSAGE( Message )
	#define PROFILER_MEM_ALLOC( Ptr, Size, ... )
	#define PROFILER_MEM_FREE( Ptr, ... )
#endif	// ENABLE_PROFILING

//-----------------------------------------------------------------------------
// Constants and types
//-----------------------------------------------------------------------------
enum profilerScopeGroup_t
{
	PROFILER_SCOPE_GROUP_NONE,
	PROFILER_SCOPE_GROUP_AI,
	PROFILER_SCOPE_GROUP_ANIMATION,
	PROFILER_SCOPE_GROUP_AUDIO,
	PROFILER_SCOPE_GROUP_DEBUG,
	PROFILER_SCOPE_GROUP_CAMERA,
	PROFILER_SCOPE_GROUP_CLOTH,
	PROFILER_SCOPE_GROUP_GAMELOGIC,
	PROFILER_SCOPE_GROUP_INPUT,
	PROFILER_SCOPE_GROUP_NAVIGATION,
	PROFILER_SCOPE_GROUP_NETWORK,
	PROFILER_SCOPE_GROUP_PHYSICS,
	PROFILER_SCOPE_GROUP_RENDERING,
	PROFILER_SCOPE_GROUP_SCENE,
	PROFILER_SCOPE_GROUP_SCRIPT,
	PROFILER_SCOPE_GROUP_STREAMING,
	PROFILER_SCOPE_GROUP_UI,
	PROFILER_SCOPE_GROUP_VFX,
	PROFILER_SCOPE_GROUP_VISIBILITY,
	PROFILER_SCOPE_GROUP_WAIT,
	PROFILER_SCOPE_GROUP_IO,
	PROFILER_SCOPE_NUM_GROUPS
};

struct profilerScopeData_t
{
	profilerScopeData_t( const char* pScopeName, const char* pFunctionName, const char* pFileName, uint32 line, profilerScopeGroup_t scopeGroup = PROFILER_SCOPE_GROUP_NONE );

	const char*			 pScopeName;
	const char*			 pFunctionName;
	const char*			 pFileName;
	uint32				 line;
	uint32				 id;
	profilerScopeGroup_t scopeGroup;
};

class CProfilerScope
{
public:
	CProfilerScope( const profilerScopeData_t& scopeData, const char* pScopeName = NULL );
	~CProfilerScope();

private:
	const profilerScopeData_t& scopeData;
	bool					   bActive;
};

class CProfilerThreadScope
{
public:
	CProfilerThreadScope( const char* pThreadName );
	~CProfilerThreadScope();
};

class CProfilerMarkScope
{
public:
	CProfilerMarkScope( const char* pMarkName );
	~CProfilerMarkScope();

private:
	const char* pMarkName;
	bool		bActive;
};

//-----------------------------------------------------------------------------
// Profiler interface
//-----------------------------------------------------------------------------
class IProfiler
{
public:
	virtual void Init()		= 0;
	virtual void Shutdown() = 0;
	virtual void Update()	= 0;

	virtual void RegisterScope( profilerScopeData_t& scopeData )								   = 0;
	virtual void BeginScope( const profilerScopeData_t& scopeData, const char* pScopeName = NULL ) = 0;
	virtual void EndScope( const profilerScopeData_t& scopeData )								   = 0;

	virtual void BeginThreadScope( const char* pThreadName ) = 0;
	virtual void EndThreadScope()							 = 0;

	virtual void BeginMarkScope( const char* pMarkName ) = 0;
	virtual void EndMarkScope( const char* pMarkName )	 = 0;
	virtual void NextFrame()							 = 0;

	virtual void SendMessage( const char* pMessage )												= 0;
	virtual void SendMemAlloc( const void* pPtr, size numBytes, const char* pAllocatorName = NULL ) = 0;
	virtual void SendMemFree( const void* pPtr, const char* pAllocatorName = NULL )					= 0;

	virtual bool IsInited() const	 = 0;
	virtual bool IsConnected() const = 0;
};

TIER0_INTERFACE IProfiler* Profiler();

#include "tier0/iprofiler.inl"
