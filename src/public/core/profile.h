#pragma once
#include "core/core.h"
#include "stdlib/math/math.h"

//-----------------------------------------------------------------------------
// Profile types and macroses
//-----------------------------------------------------------------------------
typedef void* profileDescription_t;
typedef void* profileScopeData_t;


enum profileScopeGroup_t
{
	PROFILE_SCOPE_GROUP_NONE,
	PROFILE_SCOPE_GROUP_AI,
	PROFILE_SCOPE_GROUP_ANIMATION,
	PROFILE_SCOPE_GROUP_AUDIO,
	PROFILE_SCOPE_GROUP_DEBUG,
	PROFILE_SCOPE_GROUP_CAMERA,
	PROFILE_SCOPE_GROUP_CLOTH,
	PROFILE_SCOPE_GROUP_GAMELOGIC,
	PROFILE_SCOPE_GROUP_INPUT,
	PROFILE_SCOPE_GROUP_NAVIGATION,
	PROFILE_SCOPE_GROUP_NETWORK,
	PROFILE_SCOPE_GROUP_PHYSICS,
	PROFILE_SCOPE_GROUP_RENDERING,
	PROFILE_SCOPE_GROUP_SCENE,
	PROFILE_SCOPE_GROUP_SCRIPT,
	PROFILE_SCOPE_GROUP_STREAMING,
	PROFILE_SCOPE_GROUP_UI,
	PROFILE_SCOPE_GROUP_VFX,
	PROFILE_SCOPE_GROUP_VISIBILITY,
	PROFILE_SCOPE_GROUP_WAIT,
	PROFILE_SCOPE_GROUP_IO,
	PROFILE_SCOPE_NUM_GROUPS
};


#if ENABLE_PROFILING
	#define PROFILE_INIT()				Profile()->Init();
	#define PROFILE_SHUTDOWN()			Profile()->Shutdown();
	#define PROFILE_IS_RECORD()			Profile()->IsRecord()

	// Attaches a custom data-tag to a scope
	// Supported types: int32, uint32, uint64, vec3, string (cut to 32 characters)
	#define PROFILE_TAG( Name, ... ) \
		static profileDescription_t			CONCAT( __s_pAutogenTagDescription_, __LINE__ ) = NULL; \
		if ( !CONCAT( __s_pAutogenTagDescription_, __LINE__ ) ) \
		{ \
			CONCAT( __s_pAutogenTagDescription_, __LINE__ ) = Profile()->CreateDescription( Name, __FILE__, __LINE__ ); \
		} \
		Profile()->AttachTag( CONCAT( __s_pAutogenTagDescription_, __LINE__ ), __VA_ARGS__ );

	// Scoped profiling event which automatically grabs current function name
	#define PROFILE_SCOPE( ... ) \
		static profileDescription_t			CONCAT( __s_pAutogenScopeDescription_, __LINE__ ) = NULL; \
		if ( !CONCAT( __s_pAutogenScopeDescription_, __LINE__ ) ) \
		{ \
			CONCAT( __s_pAutogenScopeDescription_, __LINE__ ) = Profile()->CreateDescription( FUNCSIG, __FILE__, __LINE__, ##__VA_ARGS__ ); \
		} \
		CProfileScope	CONCAT( __s_AutogenScope, __LINE__ )( CONCAT( __s_pAutogenScopeDescription_, __LINE__ ) ); \

	// Macro for registration a thread
	#define PROFILE_THREAD( ThreadName ) \
		CProfileThreadScope				__threadScope( ThreadName );

	// Profiling event for the main loop update
	// You need to call this function in the beginning of the each new frame
	#define PROFILE_FRAME( ThreadName ) \
		static CProfileThreadScope		__mainThreadScope( ThreadName ); \
		uint32							__frameNumber = Profile()->NextFrame(); \
		CProfileScope					CONCAT( __s_AutogenFrameScope, __LINE__ )( Profile()->GetFrameDescription() ); \
		PROFILE_TAG( "Frame", __frameNumber );
#else
	#define PROFILE_INIT()
	#define PROFILE_SHUTDOWN()
	#define PROFILE_TAG( ... )
	#define PROFILE_SCOPE( ... )
	#define PROFILE_THREAD( ... )
	#define PROFILE_FRAME( ... )
	#define PROFILE_IS_RECORD()				false
#endif // ENABLE_PROFILING


//-----------------------------------------------------------------------------
// Profiler interface
//-----------------------------------------------------------------------------
class IProfile
{
public:
	virtual void Init() = 0;
	virtual void Shutdown() = 0;

	virtual profileDescription_t CreateDescription( const achar* pFunctionName, const achar* pFileName, uint32 fileLine, const achar* pScopeName = NULL, profileScopeGroup_t scopeGroup = PROFILE_SCOPE_GROUP_NONE ) = 0;
	virtual profileDescription_t CreateDescription( const achar* pFunctionName, const achar* pFileName, uint32 fileLine, profileScopeGroup_t scopeGroup ) = 0;

	virtual void StartThreadScope( const achar* pThreadName ) = 0;
	virtual void StopThreadScope() = 0;

	virtual profileScopeData_t BeginScope( profileDescription_t pDescription ) = 0;
	virtual void EndScope( profileScopeData_t pScopeData ) = 0;

	virtual uint32 NextFrame() = 0;

	virtual void AttachTag( profileDescription_t pDescription, float value ) = 0;
	virtual void AttachTag( profileDescription_t pDescription, int32 value ) = 0;
	virtual void AttachTag( profileDescription_t pDescription, uint32 value ) = 0;
	virtual void AttachTag( profileDescription_t pDescription, uint64 value ) = 0;
	virtual void AttachTag( profileDescription_t pDescription, const vec3_t& value ) = 0;
	virtual void AttachTag( profileDescription_t pDescription, const achar* pValue ) = 0;
	virtual void AttachTag( profileDescription_t pDescription, const achar* pValue, uint16 length ) = 0;
	virtual void AttachTag( profileDescription_t pDescription, float x, float y, float z ) = 0;

	virtual profileDescription_t GetFrameDescription() = 0;
	virtual bool IsRecord() const = 0;
};

// NOTE: If the module was build with disabled profiling the function return a null implementation
CORE_INTERFACE IProfile* Profile();

//-----------------------------------------------------------------------------
// Profiler a thread scope
//-----------------------------------------------------------------------------
class CProfileThreadScope
{
public:
	CProfileThreadScope( const achar* pThreadName )
	{
		Profile()->StartThreadScope( pThreadName );
	}
	~CProfileThreadScope()
	{
		Profile()->StopThreadScope();
	}
};


//-----------------------------------------------------------------------------
// Profiler a scope
//-----------------------------------------------------------------------------
class CProfileScope
{
public:
	CProfileScope( profileDescription_t pDescription )
		: pScopeData( Profile()->BeginScope( pDescription ) )
	{}
	~CProfileScope()
	{
		if ( pScopeData )
		{
			Profile()->EndScope( pScopeData );
		}
	}

private:
	profileScopeData_t		pScopeData;
};